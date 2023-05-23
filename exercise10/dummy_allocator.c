#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "allocator.h"
#include "allocator_tests.h"
#include "membench.h"

void my_allocator_init(size_t total_size) {
	(void)total_size;
}

void my_allocator_destroy(void) {}

void* my_malloc(size_t size) {
	return malloc(size);
}

void my_free(void* ptr) {
	free(ptr);
}

void run_bench(void) {
	run_membench_global(my_allocator_init, my_allocator_destroy, my_malloc, my_free);
}

void run_tests(void) {
	test_dummy_allocator();
}
