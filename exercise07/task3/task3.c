#define _GNU_SOURCE

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if ((!defined(THREAD_PER_JOB) && !defined(THREAD_POOL)) || \
     (defined(THREAD_PER_JOB) && defined(THREAD_POOL)))
	#error "Define exactly one of THREAD_PER_JOB, THREAD_POOL"
#endif

#ifdef THREAD_POOL
	#include "thread_pool.h"
	#define POOL_SIZE 2
#else
	#include <pthread.h>
#endif

#define NUM_SEEDS 20000
#define DRAND_SUM 100000lu

typedef struct job_struct {
	long seed;
	uint64_t result;
} job_struct;

// Check how many iterations it takes until the sum of the values returned by drand48_r reaches
// DRAND_SUM. This should take about 2 * DRAND_SUM calls to drand48_r.
void* test_drand48(void* arg) {
	job_struct* job = (job_struct*)arg;
	double random_number = 0;
	double sum = 0;
	struct drand48_data drand_buffer;
	srand48_r(job->seed, &drand_buffer);
	uint64_t iterations = 0;
	while (sum < DRAND_SUM) {
		iterations++;
		drand48_r(&drand_buffer, &random_number);
		sum += random_number;
	}
	job->result = iterations;
	return NULL;
}

int main(void) {

	job_struct* jobs = malloc(NUM_SEEDS * sizeof(job_struct));
	if (jobs == NULL) {
		fprintf(stderr, "Could not allocate memory for jobs.\n");
		exit(EXIT_FAILURE);
	}

#ifdef THREAD_POOL
	thread_pool my_pool;
	pool_create(&my_pool, POOL_SIZE);
	printf("Creating thread pool with size %d.\n", POOL_SIZE);

	job_id job_ids[NUM_SEEDS];
#else
	pthread_t threads[NUM_SEEDS];
#endif
	for (size_t i = 0; i < NUM_SEEDS; i++) {
		jobs[i] = (job_struct){ .seed = i };
#ifdef THREAD_POOL
		job_ids[i] = pool_submit(&my_pool, test_drand48, jobs + i);
#else
		if (pthread_create(&threads[i], NULL, test_drand48, jobs + i) != 0) {
			perror("Creating Threads");
			return EXIT_FAILURE;
		}
#endif
	}

	uint64_t min_result = UINT64_MAX;
	uint64_t max_result = 0;
	for (size_t i = 0; i < NUM_SEEDS; i++) {
#ifdef THREAD_POOL
		pool_await(job_ids[i]);
#else
		if (pthread_join(threads[i], NULL) != 0) {
			perror("Joining Threads");
			return EXIT_FAILURE;
		}
#endif
		unsigned job_result = jobs[i].result;
		min_result = job_result < min_result ? job_result : min_result;
		max_result = job_result > max_result ? job_result : max_result;
	}

#ifdef THREAD_POOL
	pool_destroy(&my_pool);
#endif
	free(jobs);

	printf("drand48 iterations range: %" PRIu64 " to %" PRIu64 ". Expected mean: %" PRIu64 ".\n",
	       min_result, max_result, DRAND_SUM * 2);
}
