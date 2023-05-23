#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void my_allocator_init(size_t);
void my_allocator_destroy(void);
void* my_malloc(size_t);
void my_free(void*);

void run_bench(void);
void run_tests(void);

#endif
