# Exercise Sheet 11

In this exercise sheet you will explore different ways of linking and loading libraries.

## Task 1

`some_program` is an executable that should run on most `x86_64` Linux machines (and in particular it runs on ZID-GPL).
`some_library.so` is a _shared library_ used by the executable.

You can run the executable like so:

```sh
LD_LIBRARY_PATH=. ./some_program
```

Unfortunately, the library contains bugs.
While the library implementation is available in [some_library.c](some_library.c), the name of the function got corrupted.

- Use `nm` to find the name of the function (its name will tell you the intended behavior).
- Read up on how GCC can be used to compile shared libraries (i.e. _shared objects_). You should start by looking at the [linking options](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) GCC offers.
- Fix the bug in the library function.
- Write a `Makefile` to compile the revised version of `some_library.so`.
- Verify that `some_program` now behaves as expected.
- Is `some_library.so` the only shared library used by `some_program`?
  Use `ldd` to check.

Then:

- Re-implement the functionality of `some_program` and `some_library.so` in a single program `my_program`.
- Extend your `Makefile` to compile this program as a _static_ executable (again, have a look at the [linking options](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) and find out how this can be done with GCC).
- Compare the sizes of `some_program` and `my_program` — what do you find?
- Verify that your executable is actually static. Use `ldd` and `file` for this.

Equipped with this knowledge, answer the following questions:

- What is the difference between dynamic and static linking?
- When would you use dynamic linking?
- When would you use static linking?
- What is _position independent code_, and why is it useful?
- What is the purpose of the `LD_LIBRARY_PATH` environment variable?

**Note:** You can use `objdump` to disassemble a binary.
Inspecting a binary obtained from an unknown source before running it is always a good idea!

```sh
objdump --disassemble some_program --section=.text
```

**Note 2:** Linking a fully _static_ executable is not possible on ZID-GPL as a static version of the standard library is missing.

## Task 2

Dynamic linking is usually handled by the loader when running an executable.
However, in some situations a more manual approach is required.
In particular, _plugin systems_ are commonly implemented this way.

Write a program `map_string` that receives as arguments a string followed by a list of shared libraries that act as _plugins_ for the program.

The initial string is used as input for the first plugin.
The result of the first plugin is used as input for the second plugin — and so on.

```sh
LD_LIBRARY_PATH=. ./map_string "Hello world" ./double.so ./uppercase.so ./remove_whitespace.so
./double.so: Hello worldHello world
./uppercase.so: HELLO WORLDHELLO WORLD
./remove_whitespace.so: HELLOWORLDHELLOWORLD
```

Read the man pages for `dlopen(3)` and `dlsym(3)`.

Then implement the three plugins `double.so`, `uppercase.so` and `remove_whitespace.so`. Additionally, think of two different plugins similar to the ones in the example above and implement them.

## Task 3

The dynamic linker allows you to _inject_ arbitrary shared objects into any dynamically linked program using the `LD_PRELOAD` environment variable.

Use this mechanism to _wrap_ the standard library's `malloc` function in your own function, using a shared library called `malloc_spy.so`.
The wrapper function prints `allocating <size> bytes` before each allocation.
The standard library's `malloc` function is still called by your wrapper and returns the expected result.
`dlsym` provides a way to call the original function.

Try your wrapper function on an existing executable such as `ls`.
```sh
LD_PRELOAD=./malloc_spy.so ls
```

**Note:** Don't use any of the `printf` functions as these use `malloc` internally.
Format the output string manually and print it using `write`.
Here's a helper for printing numbers:

```c
void print_number(size_t number) {
    if(number > 9) {
        print_number(number / 10);
    }
    const char digit = '0' + number % 10;
    write(STDOUT_FILENO, &digit, 1);
}
```

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc11_csXXXXXX.zip
├── Makefile
├── group.txt
├── task1
│   ├── Makefile
│   ├── my_program.c
│   ├── solution.txt
│   └── some_library.c
├── task2
│   ├── Makefile
│   ├── double.c              # optional
│   ├── map_string.c
│   ├── remove_whitespace.c   # optional
│   └── uppercase.c           # optional
└── task3
    ├── Makefile
    └── malloc_spy.c
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
