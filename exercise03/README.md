# Exercise Sheet 3

## Task 1

Write a program that checks if the *environment variable* `MY_ENV_VAR` is set.
If the variable is set, the program should tell that the variable is set and show the value.
If the variable is not set, the program should also reflect that:

```
$ ./read_my_env_var
MY_ENV_VAR is not set

...

$ ./read_my_env_var
MY_ENV_VAR is set to 'Hello World'
```

Find a way to set and unset the environment variable in your shell.
Finally, find a way to set the environment variable only for a specific command (in this case `./read_my_env_var`).

You can inspect environment variables on your shell with `printenv` and printing them with `echo $MY_ENV_VAR`.
Investigate what the `PATH` environment variable is about and why it is important.

## Task 2

Create a program `calculator.c` which simulates a simple calculator. The program should accept any basic arithmetical operation as the first argument (`+`,`-`,`*`,`/`), followed by at least one number. The provided operator is then used to calculate the result using floating point arithmetic. If only one number is provided, this number is the expected result.
Consider using `strtod(3)` for parsing the input numbers.

Example output:

```bash
$ ./calculator
Usage: ./calculator <operator> <number>...
Available operators: +,-,*,/
$ ./calculator '*' 1.0
Result: 1.00
$ ./calculator '/' 12.0 2.0 3.0
Result: 2.00
```

The goal of this exercise is to perform proper _error handling_ and return according _exit codes_.

Below is a list of constraints for each available _exit code_:

- return `EXIT_SUCCESS` if the run is successful
- return `13` if too few arguments are provided (e.g.: `./calculator '+'`). In addition, print usage information.
- return `42` if the operator is unknown (e.g.: `./calculator '%' 2.0 3.0 4.0`). Again, print usage information.

For better readability, define an enum for non-standard _exit codes_.

Think of at least **two** other constraints and add appropriate _exit codes_.

How can you retrieve this _exit code_ in your shell after running the program?

In your shell `;`, `&&`, and `||` can be used to execute multiple commands on a single line. What are the differences between these 3 operators?

Answer these questions in `solution.txt`.

## Task 3

Begin by skimming through `man pthreads`.
Take note of additional compiler flags that may be required.

### Task 3a

Write a program that stores a single global variable `counter` of type `int8_t` and initialize it to `1`.

The program begins by printing the value of `counter` and then creates a child process.
The child increments the value of `counter` by `1` and exits.
The parent waits for the child to exit and prints the value again.

Next, the program (now referred to as the _main thread_) spawns a single POSIX thread.
The thread does the same thing as the child process, i.e. increment the global variable and immediately exit.
The main thread waits for the thread to finish, and prints the value one more time.

What do you notice? Explain the behavior.

Make sure to check for possible errors that might occur in the program. Look at return codes of functions like `pthread_create` and print meaningful error messages to standard error using `fprintf`

### Task 3b

Write a program that receives an arbitrary number of integers as arguments.
Let `N` be the number of arguments provided by the user.

The program creates `N` threads, each of which is assigned an ID `i`, with `0 < i <= N`.
The `i`-th thread computes a _prefix product_ of all numbers up to and including the `i`-th argument, i.e. the first thread returns the first number, the second thread computes the product of the first and second number, and so on.

The main thread then waits for all threads to complete and prints their result _in order_ in the format `"product <i> = <product>"`.

Investigate how you can pass multiple arguments to a thread function, as well as how to receive a result from it.
The program **must not** make use of any global variables.

Ensure that all threads compute their products concurrently, not one after another.

Example output:

```sh
$ ./task3b 4 3 9 5 21
product 1 = 4
product 2 = 12
product 3 = 108
product 4 = 540
product 5 = 11340
```

Again, make sure to check for possible errors that might occur in the program. Look at return codes of functions like `pthread_create` and print meaningful error messages.

_Hint_: If you are using memory in multiple threads, make sure it is available and doesn't run out of scope, while it might still be used in another thread.

--------------

Submit your solution as a zip archive via [OLAT](https://lms.uibk.ac.at/), structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc03_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task1/
│   ├── Makefile
│   ├── read_my_env_var.c
│   └── solution.txt
├── task2/
│   ├── Makefile
│   ├── calculator.c
│   └── solution.txt
└── task3/
    ├── Makefile
    ├── solution.txt
    ├── task3a.c
    └── task3b.c
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
