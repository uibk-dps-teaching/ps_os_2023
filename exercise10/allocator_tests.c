#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "allocator_tests.h"

#define POOL_SIZE (1024 * 1024 * 256)

void test_dummy_allocator(void) {
	puts("No tests available for the dummy allocator.");
}

// Assumes that at least one block can still be allocated.
// Performs a binary search for the largest allocation which succeeds.
// All memory allocated in this function is freed again.
static size_t find_maximum_block_size(void) {
	size_t lower_bound = 0;
	size_t upper_bound = SIZE_MAX;
	while (lower_bound < upper_bound) {
		size_t midpoint = lower_bound + (upper_bound - lower_bound) / 2 + 1;
		void* ptr = my_malloc(midpoint);
		if (ptr == NULL) {
			upper_bound = midpoint - 1;
		} else {
			lower_bound = midpoint;
			my_free(ptr);
		}
	}
	return lower_bound;
}

void test_free_list_allocator(void) {
	my_allocator_init(POOL_SIZE);

	const size_t block_size = find_maximum_block_size();

	const void* too_large = my_malloc(POOL_SIZE);
	assert(too_large == NULL);

	void* const ptr1 = my_malloc(1);
	assert(ptr1 != NULL);
	printf("ptr1: %p\n", ptr1);

	const void* const base = (void*)((unsigned char*)ptr1 - sizeof(void*));

	void* const ptr2 = my_malloc(1);
	printf("ptr2: %p\n", ptr2);

	const size_t header_size = (ptrdiff_t)ptr2 - (ptrdiff_t)ptr1 - block_size;
	printf("header size is: %zu\n", header_size);
	printf("block size is: %zu\n", block_size);

	my_free(ptr1);
	void* const ptr3 = my_malloc(block_size);
	memset(ptr3, 0xFF, block_size);
	printf("ptr3: %p\n", ptr3);
	assert(ptr1 == ptr3);

	void* const ptr4 = my_malloc(block_size);
	assert(ptr4 > ptr2);

	// Figure out how large entire pool is
	void* end = NULL;
	size_t num_blocks = 0;
	while (1) {
		void* next = my_malloc(1);
		if (next != NULL) {
			end = (void*)((unsigned char*)next + block_size);
			++num_blocks;
		} else {
			break;
		}
	}
	const size_t pool_size = (ptrdiff_t)end - (ptrdiff_t)base;
	// This is a lower bound, as the pool may not be evenly divisible by block + header size.
	assert(pool_size <= POOL_SIZE);

	my_allocator_destroy();

	// Test limits
	uint8_t** allocations = calloc(num_blocks, sizeof(*allocations));
	my_allocator_init(POOL_SIZE);
	// Allocate as much as possible
	for (size_t block = 0; block < num_blocks; block++) {
		allocations[block] = my_malloc(block_size);
		assert(allocations[block] != NULL);
	}
	// Check for overlaps
	memset(allocations[0], 0, block_size);
	for (size_t block = 1; block < num_blocks; block++) {
		assert(allocations[block - 1] < allocations[block]);
		memset(allocations[block], block % 0xFF, block_size);
	}
	for (size_t block = 0; block < num_blocks; block++) {
		for (size_t i = 0; i < block_size; i++) {
			assert(allocations[block][i] == block % 0xFF);
		}
	}
	// Free and reallocate some memory
	for (size_t block = 0; block < num_blocks; block += 2) {
		my_free(allocations[block]);
	}
	for (size_t block = 0; block < num_blocks; block += 2) {
		allocations[block] = my_malloc(block_size);
		assert(allocations[block] != NULL);
	}
	for (size_t block = 1; block < num_blocks; block += 3) {
		my_free(allocations[block]);
	}
	for (size_t block = 1; block < num_blocks; block += 3) {
		allocations[block] = my_malloc(block_size);
		assert(allocations[block] != NULL);
	}
	free(allocations);
	my_allocator_destroy();

	puts("All good!");
}

void test_best_fit_allocator(void) {
	my_allocator_init(POOL_SIZE);

	void* const ptr1 = my_malloc(1024);
	printf("ptr1: %p\n", ptr1);
	assert(ptr1 != NULL);
	memset(ptr1, 0xFF, 1024);

	void* ptr2 = my_malloc(1024);
	printf("ptr2: %p\n", ptr2);
	assert(ptr2 > ptr1);
	memset(ptr2, 0xFF, 1024);
	const size_t overhead = (ptrdiff_t)ptr2 - (ptrdiff_t)ptr1 - 1024;
	printf("Overhead (list header size) is %zu\n", overhead);

	my_free(ptr1);

	// Reuse first block
	void* ptr3 = my_malloc(1024);
	printf("ptr3: %p\n", ptr3);
	assert(ptr3 == ptr1);

	// Create a 2048 byte hole
	void* ptr4 = my_malloc(3072);
	memset(ptr4, 0xFF, 3072);
	void* ptr5 = my_malloc(2048);
	memset(ptr5, 0xFF, 2048);
	void* ptr6 = my_malloc(2048);
	memset(ptr6, 0xFF, 2048);
	assert(ptr5 > ptr4);
	assert(ptr6 > ptr5);
	my_free(ptr5);

	// Fill 2048 byte hole with two new allocations
	void* ptr7 = my_malloc(1024);
	memset(ptr7, 0xFF, 1024);
	void* ptr8 = my_malloc(1024 - overhead);
	assert(ptr7 == ptr5);
	assert((intptr_t)ptr8 == (intptr_t)ptr5 + 1024 + (intptr_t)overhead);

	// Check that all blocks are merged
	my_free(ptr4);
	my_free(ptr8);
	my_free(ptr7);

	void* ptr9 = my_malloc(4096);
	assert(ptr9 == ptr4);

	my_free(ptr9);
	my_free(ptr6);
	my_free(ptr2);
	my_free(ptr3);

	// Lastly, allocate all available memory
	void* ptr10 = my_malloc(POOL_SIZE - overhead);
	assert(ptr10 != NULL);

	// Check OOM result
	void* ptr11 = my_malloc(1);
	assert(ptr11 == NULL);

	my_allocator_destroy();

	puts("All good!");
}
