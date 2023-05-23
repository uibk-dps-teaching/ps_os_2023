#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "membench.h"

const size_t POOL_SIZE = 1024 * 1024 * 128;
const size_t MAX_ALLOC_MULTIPLIER = 4;
const size_t ALLOCATION_BUFFER_SIZE = 1024;

typedef struct {
	size_t num_allocations;
	size_t pool_size;
	size_t alloc_size;
	init_allocator_fn my_init;
	destroy_allocator_fn my_destroy;
	malloc_fn my_malloc;
	free_fn my_free;
	unsigned seed;
	bool init_per_thread;
} thread_context;

static int64_t get_timestamp_us(void) {
	struct timeval tv;
	const int ret = gettimeofday(&tv, NULL);
	if (ret != 0) {
		perror("gettimeofday");
		exit(EXIT_FAILURE);
	}
	return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

static void* thread_fn(void* arg) {
	thread_context* ctx = arg;
	srand(ctx->seed);
	void** ptrs = calloc(ALLOCATION_BUFFER_SIZE, sizeof(void*));

	if (ctx->init_per_thread && ctx->my_init != NULL) {
		ctx->my_init(ctx->pool_size);
	}

	size_t allocations_left = ctx->num_allocations;

	while (allocations_left) {
		for (size_t i = 0; i < ALLOCATION_BUFFER_SIZE && allocations_left; i++) {
			// Randomly free a pointer.
			if (ptrs[i] != NULL) {
				if (rand() % 2 == 0) {
					ctx->my_free(ptrs[i]);
					ptrs[i] = NULL;
				} else {
					continue;
				}
			}

			const size_t size = ctx->alloc_size * (1 + rand() % MAX_ALLOC_MULTIPLIER);
			void* ptr = ctx->my_malloc(size);

			// No more space, try randomly freeing in next iteration.
			if (ptr == NULL) {
				continue;
			}

			// printf("ptrs[%zu] = %p\n", i, ptr);

			ptrs[i] = ptr;
			allocations_left--;
		}
	}

	// Free remaining pointers.
	for (size_t i = 0; i < ALLOCATION_BUFFER_SIZE; i++) {
		if (ptrs[i] != NULL) {
			ctx->my_free(ptrs[i]);
			ptrs[i] = NULL;
		}
	}

	if (ctx->init_per_thread && ctx->my_destroy != NULL) {
		ctx->my_destroy();
	}

	free(ptrs);

	return NULL;
}

static double run_config(size_t num_threads, thread_context* ctx) {
	const int64_t before = get_timestamp_us();

	if (!ctx->init_per_thread && ctx->my_init != NULL) {
		ctx->my_init(ctx->pool_size);
	}

	pthread_t thread_ids[num_threads];
	for (size_t i = 0; i < num_threads; ++i) {
		pthread_create(&thread_ids[i], NULL, thread_fn, ctx);
	}

	for (size_t i = 0; i < num_threads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	if (!ctx->init_per_thread && ctx->my_destroy != NULL) {
		ctx->my_destroy();
	}

	const int64_t after = get_timestamp_us();

	return (after - before) / 1000.0 / num_threads;
}

static void run_membench(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free, bool init_per_thread) {
	const size_t configs[][3] = {
		{ 1, 2000, 256 }, { 10, 2000, 256 }, { 50, 2000, 256 }, { 100, 2000, 32 }
	};
	const size_t num_configs = sizeof(configs) / sizeof(size_t[3]);

	unsigned seed = time(NULL);

	for (size_t i = 0; i < num_configs; ++i) {
		const size_t num_threads = configs[i][0];
		const size_t num_allocations = configs[i][1];
		const size_t alloc_size = configs[i][2];
		const size_t pool_size = init_per_thread ? POOL_SIZE / num_threads : POOL_SIZE;

		thread_context system_ctx = { .seed = seed,
			                            .num_allocations = num_allocations,
			                            .alloc_size = alloc_size,
			                            .pool_size = pool_size,
			                            .my_init = NULL,
			                            .my_destroy = NULL,
			                            .my_malloc = malloc,
			                            .my_free = free,
			                            .init_per_thread = init_per_thread };
		thread_context custom_ctx = { .seed = seed,
			                            .num_allocations = num_allocations,
			                            .alloc_size = alloc_size,
			                            .pool_size = pool_size,
			                            .my_init = my_init,
			                            .my_destroy = my_destroy,
			                            .my_malloc = my_malloc,
			                            .my_free = my_free,
			                            .init_per_thread = init_per_thread };

		printf(
		    "%zu thread(s), %zu allocations of size %zu - %zu byte per thread. Avg time per thread:\n",
		    num_threads, num_allocations, alloc_size, alloc_size * MAX_ALLOC_MULTIPLIER);
		printf("\tSystem: %.2lf ms\n", run_config(num_threads, &system_ctx));
		printf("\tCustom: %.2lf ms\n", run_config(num_threads, &custom_ctx));
	}
}

void run_membench_global(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free) {
	run_membench(my_init, my_destroy, my_malloc, my_free, false);
}

void run_membench_thread_local(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                               malloc_fn my_malloc, free_fn my_free) {
	run_membench(my_init, my_destroy, my_malloc, my_free, true);
}
