# Exercise Sheet 4

## Task 1

In this task, we provide code which should do some string manipulation using threads.
However, this code is quite buggy.
Your task is finding the bugs in this program and fixing them.
Describe the bugs you found as comments in the source code file.
Explain what was wrong and why the fixed version is correct.

In order to make presenting the results easier,
comment out the buggy lines and add correct code,
for example:

```c
// FOUND do not compare strings with ==
// if(str1 == str2) {
if(strcmp(str1,str2) == 0) {
```

### Intended behavior

In order to fix a buggy program it is of course necessary to know what it should do.
The Intended behavior is described in this section.

All command line arguments to the program (including `argv[0]`) should be processed individually.
First, all command line arguments should be copied into heap memory.
Then, one thread per command line argument is spawned (so in total `argc` threads),
and each thread works on a single one of these arguments.

Each thread receives a pointer to the copy of the argument it should work on.
The job of the thread is reversing this argument (into a new array),
and converting the input string to upper case (in-place).
In order to make the reversed string accessible to the main thread,
a pointer to it should be returned by the thread.

Once the main thread is done spawning the threads, it waits for their completion
and for each completed thread, the original argument it received, the upper-cased version,
and the reversed argument are printed.

### Debugging

The bugs in this program are mostly issues with memory accesses, such as accessing addresses which are not part of an allocation or leaking dynamically allocated memory.

One tool which can be very helpful for finding such issues is `valgrind`.
This tool can run a binary and check if it accesses any invalid memory addresses or leaks memory. In order to get more informative messages, make sure to pass the flag `-g` to the C compiler.

A run of `valgrind` on the corrected version of the program might look like this:

```text
$ valgrind ./task1 some Arguments 42
==5437== Memcheck, a memory error detector
==5437== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==5437== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==5437== Command: ./task1 some Arguments 42
==5437==
original argv[0]: ./task1
uppercased: ./TASK1
reversed: 1ksat/.
original argv[1]: some
uppercased: SOME
reversed: emos
original argv[2]: Arguments
uppercased: ARGUMENTS
reversed: stnemugrA
original argv[3]: 42
uppercased: 42
reversed: 24
==5437==
==5437== HEAP SUMMARY:
==5437==     in use at exit: 0 bytes in 0 blocks
==5437==   total heap usage: 12 allocs, 12 frees, 2,292 bytes allocated
==5437==
==5437== All heap blocks were freed -- no leaks are possible
==5437==
==5437== For lists of detected and suppressed errors, rerun with: -s
==5437== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Note the absence of any error messages and memory leaks.
For debugging, it might be helpful to pass flags to `valgrind`,
in particular `--track-origins=yes`, `--leak-check=full`, and `--show-leak-kinds=all`.

It might also be helpful to use a debugger, either integrated in your editor/IDE or a standalone debugger like `gdb`.

### Optional bonus part

Use `valgrind` to check if there are any memory leaks or other issues in your queue implementation from the first week.
You do not have to submit anything for this part, it is just meant for you to practice and maybe find out about issues with your code.

## Task 2

On the shell you can provide the output of a command as input to another using a _pipe_ `|`.

```
$ ls | wc -l
7
```

This _forwards_ the standard output of `ls` to the standard input of `wc` (word count).
The result you see is the output of `wc -l`, the number of lines.
Multiple commands can be chained together to build longer _pipelines_.

```
$ ls -l | grep ^d | wc -l
3
```

Your task is to create a C program `ls_wc` that uses `fork` and `exec` to run the two programs `ls` and `wc` (with the argument `-l`) like just shown.
Have a look at the `fork(2)`, `exec(3)`, `pipe(7)`, `pipe(2)`, and `dup(2)`/`dup2(2)` man pages.

Then, create a program `ls_grep_wc` that mimics the second example.
Your Makefile should build both executables.

There are even more helpful operators built into your shell.
Explain what the following operators do: `<`, `>`, `2>`, `2>&1`

## Task 3

In this task you will work with FIFOs, also called **named** pipes.

The goal is to write a simple “math server” where multiple interactive **clients** can send math expressions to a **server** which will then evaluate them and print the result to the console.
Your solution should produce two separate executables, `math_client` and `math_server`.

### Server

1. The **server** can be started with a list of clients that may connect to it, e.g. after starting with `./math_server jacobi riemann`, two clients named `jacobi` and `riemann` can connect.
2. The server then creates a FIFO for each client. Think about a way of associating each FIFO with a specific client.
   **NOTE**: If you have problems creating FIFOs, make sure that your filesystem actually supports them (this can be a problem in WSL when working on an NTFS drive). A good location to store FIFOs is somewhere inside `/tmp` (when working on ZID-GPL, be sure to avoid file naming collisions with other students).
3. The server then waits for clients to connect by opening each FIFO for reading. There is no need to use `fork(2)` in this task, you can assume that clients connect in the same order they are specified on the command line.
4. Once a client connects, the server prints the message `"<client name> connected."`.
5. The server continuously monitors all FIFOs for incoming math expressions using `poll(2)`.
   1. Once an expression is received, it is evaluated and printed to the console like so `"<client name>: <expression> = <result>."`.
      The server only needs to support very simple math expressions of the form `<a> <op> <b>` where `a` and `b` are real numbers and `op` is one of `+-*/`.
      If an expression does not conform to the expected format, the server instead prints `"<client name>: <expression> is malformed."`.
   2. If a client disconnects, the server prints the message `"<client name> disconnected."`.
6. Once all clients are disconnected, the server cleans up the FIFOs and exits.

### Client

1. A **client** is started by providing it with a name, e.g. `./math_client jacobi`.
2. The client will then attempt to connect to the server by opening one of the FIFOs for writing.
3. Once connected, the client continuously prompts for a math expression to send.
4. Each expression is then written to the FIFO. You can assume a math expression to be at most `PIPE_BUF` long. Why is this important?
5. If the expression is empty, the client disconnects by closing the FIFO and exiting.

Here is an example session (each line represents a different point in time, `>` indicates user input).

```
TERMINAL 1                     │ TERMINAL 2             │ TERMINAL 3
───────────────────────────────┼────────────────────────┼─────────────────────────
> ./math_server jacobi riemann │                        │
                               │ > ./math_client jacobi │
jacobi connected.              │ Expression:            │
                               │                        │ > ./math_client riemann
riemann connected.             │                        │ Expression:
                               │ > 1.359 * 2            │
jacobi: 1.359 * 2 = 2.718      │ Expression:            │
                               │                        │ > 1.139 - 0.304
riemann: 1.139 - 0.304 = 0.835 │                        │ Expression:
                               │ > 2 * i                │
jacobi: 2 * i is malformed.    │ Expression:            │
                               │ > (empty)              │
jacobi disconnected.           │                        │
```

Additional notes and hints:

- You can assume that all clients are connected before handling their messages.
- Your server doesn't need to support clients re-connecting, i.e. they can (and must) connect exactly once.
- Your solution **must** use `poll(2)`, **not** `select(2)`.
- Make sure to use appropriate file permissions (which we discussed in connection to `chmod`) to create and open your FIFOs.
  Justify your choice.
- Your FIFOs should be **blocking**, i.e. you **must not** use `O_NONBLOCK` in your calls to `open(2)`.
- You can use the `%g` format specifier to let `printf(3)` decide on the number of significant digits to print.
- As always, make sure to properly release all allocated resources (e.g. FIFOs) upon exiting.
- On macOS, `poll(2)` is bugged and will not return when you close the pipe from the client,
  run on ZID-GPL to ensure the correct behaviour.
- If you want to create multiple processes side-by-side for testing, there are several different options available to you:
  1. Open multiple terminal windows or tabs.
  2. Use shell jobs to switch between different processes: <kbd>CTRL</kbd> + <kbd>Z</kbd>, `&`, `fg`, `bg`, `jobs`, etc.
  3. Use a terminal multiplexer such as `tmux` or `screen`

--------------

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```
exc04_csXXXXXX.zip
├── group.txt            # optional
├── task1
│   ├── Makefile
│   └── task1.c
├── task2
│   ├── Makefile
│   ├── ls_grep_wc.c
│   ├── ls_wc.c
│   └── solution.txt
└── task3
    ├── Makefile
    ├── math_client.c
    ├── math_common.h    # optional
    ├── math_server.c
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
