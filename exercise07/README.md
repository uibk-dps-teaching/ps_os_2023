# Exercise Sheet 7

## Task 1

In this task you'll simulate a simple dice rolling game using **only** *pthread barriers* for synchronization.
Start by reading the man pages on how to use barriers, `man 3 pthread_barrier_init` and `man 3 pthread_barrier_wait`.

The game is started with `n` players, specified as a command line argument.
Each round all of the players roll a die (generate a random number between 1 and 6 for this).
The player(s) with the smallest score are then eliminated from the game.
If all remaining players roll the same number, the round is repeated until a winner has emerged.
The game is finished if only one player is left.

- The main thread **only** does setup and cleanup.
- Each player is simulated by one dedicated thread.
- One of the players (**not the main thread**) determines which player(s) are eliminated.
  Use `pthread_barrier_wait`'s return value for this.
- You **must not** use any global variables!
- It might be helpful to define a struct to combine multiple arguments that are passed to a thread function.
- You might also want to use a struct to save your game state. This could look something like this:

```c
typedef struct {
  uint32_t n;
  volatile int rolls[];
} game_state;
```

Example output where player 3 wins the game:

```
Player 0: rolled a 4
Player 1: rolled a 1
Player 2: rolled a 5
Player 3: rolled a 3
Player 4: rolled a 2
Player 5: rolled a 1
Eliminating player 1
Eliminating player 5
---------------------
Player 0: rolled a 4
Player 2: rolled a 3
Player 3: rolled a 5
Player 4: rolled a 2
Eliminating player 4
---------------------
Player 0: rolled a 5
Player 2: rolled a 1
Player 3: rolled a 6
Eliminating player 2
---------------------
Player 0: rolled a 3
Player 3: rolled a 3
Repeating round
---------------------
Player 0: rolled a 1
Player 3: rolled a 3
Eliminating player 0
---------------------
Player 3 won the game!
```

**Note:** The pthread barrier functionality will only compile if you define the right feature test macro. In this case, you can define `-D_POSIX_C_SOURCE=200112L` when compiling your code.

## Task 2

Continuing with [last week's task 1](../exercise06/README.md), where you synchronized the program using **pthread mutexes**. In this task you will implement your own mutex using atomics. Your solution should at least provide two functions: `my_mutex_lock` and `my_mutex_unlock`. You may come up with additional functions, if needed.

Think about ways of representing the state of your mutex using an atomic variable. Instead of manipulating that variable directly however, use the `atomic_flag_test_and_set` and `atomic_flag_clear` functions (why is this necessary?). Alternatively, if you prefer, you can also use `atomic_compare_exchange_weak`. Make sure you fully understand the behaviour of these functions and how they could be used to implement your mutex before proceeding.

We want to compare the performance of our *own mutex* and the *pthread mutex variant* from last week. If you did not submit the *pthread mutex variant* (exercise 6 task 1), you should implement it throughout this task. An elegant way to provide both implementations while minimizing code duplication could be to use preprocessor macros, like this:

```c
#if USE_MY_MUTEX
    #define MUTEX_LOCK my_mutex_lock
    // ... other functions
#else
    #define MUTEX_LOCK pthread_mutex_lock
    // ... other functions
#endif
```

You can then use the `MUTEX_LOCK` macro for both variants of the lock function, and switch between the two implementations by adding `-DUSE_MY_MUTEX=<0 or 1>` to your compiler call.

Measure the execution time with `/usr/bin/time -v`. Report your findings and interpret the results. Make sure to use an optimized build (`-O2` or `-O3`).

**Hints:**

- Locking your mutex will likely require a loop of some sort.
- It may be helpful to `assert` your expectations about the state of the mutex within your lock/unlock functions.
- *Optional*: If you want to improve the performance of your mutex, take a look at the `sched_yield` function.

## Task 3

In this exercise you will implement a [`thread pool`](https://en.wikipedia.org/wiki/Thread_pool), containing a limited number of worker threads.

The provided code in [`task3.c`](task3/task3.c) computes a fixed amount of jobs (it is not important to understand what the `test_drand48` function does).

The file contains a version which spawns several threads. Compile the code with `make task3a` and run it. Find out how many threads it spawns.

This file also contains a version which uses a thread pool interface, which is defined in [`thread_pool.h`](task3/thread_pool.h).
Compiling this (`make task3b`) will not work because the thread pool functions are not implemented yet.
More information and additional requirements about the thread pool implementation can be found in [`README.md`](task3/README.md).

**Note**: The provided values for `NUM_SEEDS` and `DRAND_SUM` should be used for benchmarking, but you might want to use smaller values during development.

Complete the following tasks:

- Choose suitable data types for the thread pool implementation and implement the functions in [`thread_pool.c`](task3/thread_pool.c).
- Experiment with different thread pool sizes (change the `POOL_SIZE` constant). Which size results in the best performance and why?
- Compare the performance of `task3a` and `task3b` (with the optimal thread pool size) by benchmarking them with `/usr/bin/time -v` and report your findings.
- Answer these questions:
  - What are the advantages of using a thread pool rather than spawning a new thread for each job?
  - In which situations does it make sense to use a thread pool?

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc07_csXXXXXX.zip
├── group.txt            # optional
├── task1
│   ├── Makefile
│   └── task1.c
├── task2
│   ├── Makefile
│   ├── solution.txt
│   └── task2.c
└── task3
    ├── Makefile
    ├── solution.txt
    ├── thread_pool.h
    ├── thread_pool.c
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
