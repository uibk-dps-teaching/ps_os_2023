# Exercise Sheet 5

## Task 1

In this task, you will develop a multi-process application using shared memory for communication.

Begin by reading `man 7 shm_overview`. Take note of additional linker flags that may be required.

---

The program receives three integers as arguments of type `uint64_t`:

- `N`, an arbitrary integer
- `K`, number of reads/writes to the buffer
- `B`, the length of the circular buffer (total size: `B * sizeof(uint64_t)`)

First, each input argument is parsed (as discussed in the lab). Then, it sets up shared memory for communication. It contains a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) and one element for the result. We provided you with a sample struct. Feel free to use it. 

The circular buffer allows the number of reads/writes defined by `K` to be larger than the number of elements `B` in the buffer. If the end is reached, the process restarts at the front (using `buffer[i % B]`, where `i` is the number of iterations done so far).

Next, two child processes are created. 
The processes run in parallel and perform calculations on the buffer:

- **Child A**: 
  - The process loops `K` times. In each iteration `i`, the number `N * i` is written into position `i % B` of the circular buffer.
  - It then finishes up, and returns success
- **Child B**: 
  - The process computes the sum of each element in the circular buffer. It prints the final result, and writes it into the result element in the shared memory. 
  - It then finishes up, and returns success

The following struct might help:

```c
struct TODO {
    uint64_t result;
    uint64_t buffer[];
};
```

In this struct, `buffer` is a so-called [flexible array member](https://en.wikipedia.org/wiki/Flexible_array_member).

The parent process waits for the termination of both child processes. It reads the result of their computation from the result element in the shared memory. It then validates the result of the computation using the following function.
It then finishes up, and returns success. 
```c
void validate_result(uint64_t result, const uint64_t K, const uint64_t N) {
    for (uint64_t i = 1; i <= K; i++) {
        result -= N * i;
    }
    printf("Checksum: %lu \n", result);
}
```

### Example outputs

```sh
$ ./task1 32044 3042 3040
Result: 148312545732
Checksum: 0
```

```sh
$ ./task1 32044 3042 3040
Result: 148507373252
Checksum: 194827520
```


You will notice that the result is not always correct. This is by design and does not indicate an implementation mistake.

- Test your implementation with different values for `N`, `K`, and `B`. 
- What is the checksum indicating? 
   - Which values are correct vs. incorrect
   - How can the checksum be used to tell if a result is correct or not?

Additional notes and hints:

> ℹ️ **Macro support**:
>
> In order to use `sigaction`, `usleep`, or macros such as `MAP_ANONYMOUS` you have to define `_POSIX_C_SOURCE` and `_BSD_SOURCE` feature test macros.
> This can be done by adding the compile flag `-D_POSIX_C_SOURCE -D_BSD_SOURCE`.
> See the `feature_test_macros(7)` man page for more information.
> On modern systems the `_BSD_SOURCE` feature test macro has been deprecated and replaced by `_DEFAULT_SOURCE`, however this is not available on zid-gpl.
> You may ignore the deprecation warning that is emitted when using `_BSD_SOURCE`, however your program should not generate any other warnings.

- Make sure to perform proper error handling and input parsing
- Test your implementation using `valgrind`
  - Did you clean everything up?
  - Do you have access to everything you need?
  - Is everything correctly initialized?

## Task 2

In this task, you will use _semaphores_ to synchronize the access to the shared data structure.

This exercise MUST be implemented using the POSIX semaphore facilities.

### Task 2a

Implement the same program as in the previous task 1. This time however, ensure that the checksum is always correct by utilizing synchronization concepts.

Use two semaphores to implement this behavior.

Your program should now always compute the correct checksum.

**Important**: Make sure that the processes can perform work on the circular buffer _concurrently_, i.e., the semaphores must be used for _counting_.

Measure the execution time of your solutions for Task 1 and Task 2a using `/usr/bin/time`.
What do you notice?

### Task 2b

Answer the following questions:

- What is the difference between shared memory and e.g. a global variable after calling `fork()`?
- What is a race condition? Why do race conditions occur?
- Is the output of a program with a race condition always incorrect?
- If you run your program 100 times and always get the correct result, does that mean that there is no race condition?
- What is synchronization?
- Why are semaphores well suited for synchronizing such programs?
- There are two ways of creating a POSIX semaphore.
  Which would you use when?

## Task 3

In this task we will look at **POSIX** message queues — see `mq_overview(7)`.

The goal of this task is to simulate a random number generator, with multiple requests being processed according to their priority.
The simulation consists of two executables: The `random_server` receives requests via a **message queue** and "prints" generated random numbers by writing them to `stdout`.
One or more clients, simply called `random`, submit requests to the message queue.

Server behavior:

- The server receives the name of the message queue as its only argument.
  To avoid naming collisions with other students on ZID-GPL, we recommend naming your queue based on your user name.
- It creates the message queue and continuously waits for incoming requests, taking priorities into account.
- Upon receiving a job, it prints `Serving request with priority <priority>:`, followed by the generated random numbers.
- Simulate long computation times by sleeping 500 milliseconds before printing each random number.
  Make sure to flush `stdout` to see them being printed one by one.
- Find a way of **gracefully** shutting down the server.
  You may find _"signaling"_ inspiration in earlier exercise sheets.
  Upon shutdown, the server discards all outstanding jobs, prints `Shutting down.`, cleans up and exits.

The client receives the message queue name as well as a _priority_ as its arguments.
It then opens the message queue created by the server, sends a request with its contents read from `stdin` and then immediately exits.
The request should contain three numbers, lower bound, upper bound and the amount of random numbers required.

Example output:

```text
┌──────────────────────────────────┬────────────────────────────────────────┬────────────────────────────────────────┐
│TERMINAL 1                        │TERMINAL 2                              │TERMINAL 3                              │
├──────────────────────────────────┼────────────────────────────────────────┼────────────────────────────────────────┤
│$ ./random_server "/csXXXX"       │                                        │                                        │
│                                  │$ echo "1 100 5" | ./random "/csXXXX" 1 │                                        │
│Serving request with priority 1:  │                                        │                                        │
│76      59      93      82      86│                                        │                                        │
│                                  │                                        │$ echo "1 100 5" | ./random "/csXXXX" 10│
│                                  │$ echo "1 100 5" | ./random "/csXXXX" 99│                                        │
│Serving request with priority 99: │                                        │                                        │
│79      69      91      88      56│                                        │                                        │
│Serving request with priority 10: │                                        │                                        │
│82      1       48      17      62│                                        │                                        │
└──────────────────────────────────┴────────────────────────────────────────┴────────────────────────────────────────┘
```

In this example, the server starts generating numbers for the request with priority 1, since it is the first to be received.
While this computation is done, 2 other requests are added to the queue.
After the first request has been completed, the server takes the next message (according to priority) from the queue.

Additional notes:

- Pick a reasonable message size
- Ensure the message queue is properly cleaned up on shutdown
- Make sure to use appropriate file access modes

Answer the following questions:

- What is the range of possible priorities defined by POSIX?
  - Are you limited to this range on your system?

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc05_csXXXXXX.zip
├── group.txt            # optional
├── task1
│   ├── Makefile
│   ├── task1.c
│   └── solution.txt
├── task2
│   ├── Makefile
│   ├── task2.c
│   └── solution.txt
└── task3
    ├── Makefile
    ├── random.c
    ├── random_server.c
    └── solution.txt
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
