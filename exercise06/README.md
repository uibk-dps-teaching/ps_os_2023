# Exercise Sheet 6

## Task 1

Write a program that initializes an integer variable `counter` to 0 and subsequently creates 1000 POSIX threads.

Each thread should execute a loop of 20000 iterations. In each iteration `i`, the value of `counter` is
  - incremented by 2, if `i` is even, or
  - decremented by 1, if `i` is odd.
  
The main thread waits for all other threads and then prints the final value of `counter`, which should be 10000000.

To prevent race conditions, implement two variants:

- Protect access to `counter` using a mutex. (Threads must be able to make progress concurrently.)
- Use atomics for decrementing and incrementing `counter`.

Answer the following questions:

- How does the program behavior differ between the two variants? Measure the execution time of both using `/usr/bin/time -v` and report your findings. Make sure to use an optimized build (`-O2` or `-O3`).
- What is the effect of specifying optimization flags when compiling?
- How do `-O2` and `-O3` differ?
- What is the difference between using `+=` on a normal vs. an atomic integer type?
- How does using e.g. `+=` on atomic types relate to using atomic functions such as `atomic_fetch_add`?
- Which operations other than decrementing/incrementing can be done atomically?

**Hint**: Due to an issue with the GCC version on ZID you might get the error `fatal error: atomic.h: No such file or directory`. To solve this issue, load a more recent compiler version with `module load gcc/9.2.0`. Note that loading modules breaks man pages, which can be fixed with `export MANPATH=":$MANPATH"`.

## Task 2

In this task you will revisit the producer-consumer pattern, this time with multiple consumers implemented through POSIX threads.

Your program should take 2 command line arguments: The first argument is the number of _consumers_ `c`, which all try to read elements from a queue storing integers, and the second argument is the number of elements `n` which should be put into the queue.

For the queue, you can use the basic implementation that is provided in [`myqueue.h`](task2/myqueue.h).

- When a consumer thread successfully reads an element, it adds it to its local `sum`.
  When the element is `-1`, it prints out the sum, returns it to the main thread and exits.
- The main thread acts as the _producer_.
  After spawning the `c` consumers, it feeds `n` entries of value `1` into the queue, followed by `c` entries of value `-1`.
- The main thread then waits until all consumers have finished and computes the final sum from all the partial results, prints it to the console and exits.

To avoid race conditions, synchronize access to the queue by using the `pthread_mutex` facilities.
Check the appropriate man pages (`pthread_mutex_lock(3p)`, `pthread_mutex_destroy(3p)`, ...).
Carefully consider all locations where you might need synchronization.

**Important**: The consumer threads should be able to begin processing elements from the queue as soon as the producer has added them, NOT only once all elements have been added.

Example output:

```text
$ ./task2 5 10000
Consumer 2 sum: 2165
Consumer 1 sum: 1501
Consumer 4 sum: 2320
Consumer 0 sum: 2219
Consumer 3 sum: 1795
Final sum: 10000
```

Instead of a mutex, could a semaphore be used in this situation?

## Task 3

Continuing with the single producer, multiple consumer example, in this exercise you should improve the performance of your implementation from the previous task by using a pthread _condition variable_ to signal the availability of new elements to the consumers.

Answer the following questions:

- What is the advantage of using a condition variable in this case, compared to using plain mutexes?
- When would you use condition variables?
- What are spurious wakeups in the context of condition variables, and how can they be mitigated?
- How does the program behavior differ between the two variants?

Benchmark the following commands and make sure to use an optimized build (`-O2` or `-O3`). Report your findings.

```text
/usr/bin/time -v ./task2 250 100000
/usr/bin/time -v ./task3 250 100000

/usr/bin/time -v ./task2 500 100000
/usr/bin/time -v ./task3 500 100000

/usr/bin/time -v ./task2 1000 100000
/usr/bin/time -v ./task3 1000 100000
```

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc06_csXXXXXX.zip
├── group.txt            # optional
├── task1
│   ├── Makefile
│   ├── solution.txt
│   ├── task1_atomic.c
│   └── task1_mutex.c
├── task2
│   ├── Makefile
│   ├── myqueue.h
│   ├── solution.txt
│   └── task2.c
└── task3
    ├── Makefile
    ├── myqueue.h
    ├── solution.txt
    └── task3.c
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
