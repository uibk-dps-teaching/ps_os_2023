# Exercise Sheet 10

In this exercise sheet you will implement _custom allocators_ and compare their performance to the system allocator.

## Task 1

In some cases the system allocator (`malloc()` / `free()`) may not be suitable (for example in embedded programming) or may not perform well for atypical allocation patterns (for example in video games and other highly optimized programs).
In these situations, it can be beneficial to implement a _custom allocator_.

One of the simplest allocators is the so-called _free-list allocator_, which you will implement in this task.
Free-list allocators come in different forms, but the most basic variant allows to allocate _blocks_ of a fixed size from a given area of memory, called the _memory pool_.

As the name implies, a free-list allocator consists of a linked list of free blocks which are available to be allocated.
Importantly, this list is embedded within the memory pool itself, i.e., lives inside the very memory region that is being managed for the user (this is called an _intrusive data structure_).
More specifically, each block should be preceded by a _header_ that contains a pointer to the next free block in the list.
This diagram illustrates how your free-list may look like initially:

```text
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │  Free  │   │  Free  │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └▲─┬┴────────┴▲─┬┴────────┴▲─┬┴────────┴▲─┬┴──────
    └───────┘ └──────────┘ └──────────┘ └──────────┘ └───────
```

The user can then allocate one block at a time.
After the first two blocks have been allocated, the list may look like this:

```text
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │ Allocd │   │ Allocd │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └───┴────────┴───┴────────┴▲─┬┴────────┴▲─┬┴──────
    └─────────────────────────────────┘ └──────────┘ └───────
```

If the first block is then free'd again, its header will point to the third block, which is the next free block:

```text
           │------------------ Memory pool ------------------
           ┌───┬────────┬───┬────────┬───┬────────┬───┬──────
           │   │        │   │        │   │        │   │
Next Free  │   │  Free  │   │ Allocd │   │  Free  │   │  ...
    │      │   │        │   │        │   │        │   │
    │      └▲─┬┴────────┴───┴────────┴▲─┬┴────────┴▲─┬┴──────
    └───────┘ └───────────────────────┘ └──────────┘ └───────
```

To obtain a pool of memory that can then managed by your allocator, use `mmap` with the `MAP_PRIVATE` and `MAP_ANONYMOUS` flags.
You can do this once during initialization; it should be the only time your custom allocator interacts with the operating system's memory management facilities (except when releasing the pool at the end).
**Important**: Your solution must not use the system allocator (so no `malloc`, `calloc`, `realloc`, `reallocarray`, `free`).
**Note**: An alternative to `mmap` could be to use `brk`/`sbrk` to obtain memory usable by the allocator.
          If you are interested how real-world allocators are implemented, have a look at their documentation or source code.
          For an introduction, have a look at this brief description of the `malloc` implementation in the GNU C Library: [https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html](https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html) (This is entirely optional.)

Your allocator should implement the functions defined in [`allocator.h`](./allocator.h):

- `void* my_malloc(size_t size)` allocates a single block from the list, returning its address.
  If no free block is available, it returns `NULL`.
  While the signature of this function mimics `malloc()`, the `size` parameter does not influence the size of your blocks (they all have the same fixed block size).
  Return `NULL` if `size` exceeds the size of a single block.
- `void my_free(void* ptr)` frees a block, returning it to the list.
- `void my_allocator_init(size_t size)` initializes the memory pool for the allocator to a total size of `size`.
  Your allocator can at most allocate however many blocks (including list headers) fit into `size`.
- `void my_allocator_destroy(void)` frees the memory pool for the allocator.
- `void run_bench(void)` runs benchmarks for your allocator. See [`dummy_allocator.c`](./dummy_allocator.c) and [`membench.h`](./membench.h).
- `void run_tests(void)` runs tests for your allocator. See [`dummy_allocator.c`](./dummy_allocator.c) and [`allocator_test.h`](./allocator_tests.h).

To help you with testing and benchmarking, we provide several files:

- [`allocator_tests.c`](./allocator_tests.c) contains a function `test_free_list_allocator` that checks whether your implementation is behaving correctly.
  We recommend using this during development.
- [`membench.c`](./membench.c) is a benchmarking program designed to evaluate the performance of your allocator against the system allocator.
  Use the `run_membench_global` function to run the benchmark.
  Briefly interpret the results.
- [`dummy_allocator.c`](./dummy_allocator.c) shows how an allocator should be integrated into the project.
  In particular, the functions declared in [`allocator.h`](./allocator.h) must be defined by your allocator.
  In order to build your allocator, you will have to add a rule in the provided [`Makefile`](./Makefile).
  The easiest way to do this is to copy the `dummy_allocator` rule, modifying the target name and the first prerequisite,
  and adding the new target to `TARGETS`. (Just put a space and then the name of the target after `TARGETS = dummy_allocator`.)

Your solution should allow running both the tests and the benchmark without manual changes to the code or Makefile (i.e., produce multiple executables or implement a command line parameter).

**Additional notes and requirements:**

- Assume a fixed block size of `BLOCK_SIZE = 1024` bytes.
- Your allocator must be thread-safe. Make sure to properly synchronize critical sections.
- Because the function signatures do not allow passing around a struct,
  you need to find another way of storing the data you need for managing allocations.
  Because this data is an implementation detail of your allocator, it should not be visible in other translation units.
  This can be achieved by using top-level `static` variables: `static bool example;`
- Make sure to use Valgrind to find potential errors in your implementation.
- There should be no need to modify any of the provided files, except for the [`Makefile`](./Makefile) where you should add a rule for your allocator.
- Use the `test_free_list_allocator` function from `allocator_tests.c` to check whether your implementation is behaving correctly.
- Benchmark the performance of your allocator using `run_membench_global()`, by calling this function in `run_bench()`
  and running your executable with `bench` as the only argument.

## Task 2

One of the shortcomings of the simple free-list allocator from Task 1 is that it can only allocate blocks of a fixed size.
To alleviate this problem, turn your allocator into a _best fit_ allocator that supports blocks of varying size.

The basic working principle remains the same, however your list initially starts out with only one huge block that spans the entire memory pool.
Once an allocation of a given `size` is made, the block is split into two:
One part of size `size` is returned to the user, while the remainder is reinserted into the free list.

Given that the free-list now consists of blocks of varying size, your allocator should not simply allocate the first block from the free-list.
Instead, it should try and find the block in the list whose size is closest to the requested `size` (hence the name "best fit" allocator).

Again:

- Use the `test_best_fit_allocator` function from `allocator_tests.c` to check whether your implementation is behaving correctly.
- Benchmark the performance of your allocator using `run_membench_global()`, by calling this function in `run_bench()`
  and running your executable with `bench` as the only argument.

**Additional notes:**

- Your list headers will no longer be spaced evenly throughout the memory pool.
  Instead, you have to create them dynamically based on the number of blocks (and their sizes) currently in the list.
- Additionally, headers will now need to store more information than simply a pointer to the next block.
- Keep the number of entries in the free-list as small as possible.
  Once a block is free'd, check whether it can be merged with any of its neighboring blocks.

## Task 3

For this task, instead of a single global allocator that requires synchronization to be thread-safe, create a separate _thread local_ allocator for each thread that uses it.
To do so, you can leverage C11's thread local storage class specifier (the `_Thread_local` keyword).

Here, a thread local allocator is an allocator which is only used by a single thread, and the allocated memory is not accessed or free'd by any other thread.
Therefore, each thread can do its allocations independently of the other threads, which should allow your implementation to have better performance than in the previous tasks.

Convert the allocator you implemented in Task 1 or Task 2 to a thread local allocator, without modifying the behavior otherwise.
Measure the performance, now using the `run_membench_thread_local()` function, and compare it to the performance of the corresponding global allocator.

---

There are no prescribed file names for your allocators,
as you may choose to have the code for task 3 in the same source file as the code for task 1 or 2.

```text
exc10_csXXXXXX.zip
├── Makefile
├── group.txt
├── task1
│   ├── Makefile
│   ├── allocator.h
│   ├── allocator_tests.c
│   ├── allocator_tests.h
│   ├── free_list_allocator.c
│   ├── main.c
│   ├── membench.c
│   └── membench.h
├── task2
│   ├── Makefile
│   ├── allocator.h
│   ├── allocator_tests.c
│   ├── allocator_tests.h
│   ├── best_fit_allocator.c
│   ├── main.c
│   ├── membench.c
│   └── membench.h
└── task3
    ├── Makefile
    ├── allocator.h
    ├── allocator_tests.c
    ├── allocator_tests.h
    ├── best_fit_allocator.c      # optional if implemented free-list allocator
    ├── free_list_allocator.c     # optional if implemented best-list allocator
    ├── main.c
    ├── membench.c
    └── membench.h
```

Requirements

- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Check your file structure
- [ ] Submit zip
- [ ] Mark solved exercises in OLAT
- [ ] Any implementation MUST NOT produce any additional output
- [ ] If you work in a group, create a `group.txt` file according to the format specified below.

If you worked in a group, the `group.txt` file must be present
and have one line per student which contains the matriculation number
in the beginning, followed by a space and the student's name.
For example, if the group consists of Jane Doe,
who has matriculation number 12345678,
and Max Mustermann, who has matriculation number 87654321,
the `group.txt` file should look like this:

```text
12345678 Jane Doe
87654321 Max Mustermann
```
