# Exercise Sheet 1

This exercise sheet contains the self-assessment exercise along with some introductory exercises to using the command-line.

## Task 1

This first task does not require the use of any operating systems concepts besides what you have already learned in the previous semester.
Instead, the purpose of this task is to check (for yourself) whether you are sufficiently fluent in using the C programming language.
Most exercises in this course will build directly on top of what you've learned in the previous semester.
If you are struggling with this task, consider doing this course a year later, familiarizing yourself with the C Programming language in the meantime.
The workload might otherwise be higher for you than originally planned by us.


The first task requires you to write a queue for `integer` values.
The queue **must be** of dynamic size and you are not allowed to change the `myqueue_test.c` file.
A queue is a list, in which new elements are added to the end.
The values are returned as _first come, first served_
We provide you a header and testfile.
- Complete the header file
  - `myqueue_init` is the initialization function. It makes sure your queue is ready for action
  - `myqueue_is_empty` checks that the queue is empty
  - `myqueue_enqueue` adds a new value to the end of the queue
  - `myqueue_dequeue` returns the first value from the queue
    - Make sure a user can't return a value from an empty queue
  - `myqueue_destroy` frees not required resources
    - afterwards the queue needs to be initialized again to be used
    - there is no more trace in the memory of the queue
- Use the testfile to check your implementation
  - Do Not change the testfile

Example output:

```
$ ./myqueue_test
    All good!
```


## Task 2

As a computer scientist you will likely spend a lot of your time in a terminal.
It is therefore a good idea to get accustomed to a few basic commands.
We will gradually introduce new commands and concepts over the semester.

For now, you should teach yourself how to do the following tasks.
Introductory [slides from the previous year](../lab01/1_history_and_shell.pdf) are provided, but you are free to use any tutorial you find.

- Connecting to ZID-GPL via `ssh`
  - See [LPCCS System](https://www.uibk.ac.at/zid/systeme/linux/)
  - If the `ssh` command is not available on your system, install the *OpenSSH Client*
    - Windows: Settings > Apps & features > Optional features > OpenSSH Client
    - Ubuntu: `sudo apt install openssh-client` in your terminal
  - You can close the connection by logging out with the `exit` command

- Looking around, changing directory (`pwd`, `ls`, `cd`)
- Creating files and directories (`touch`, `mkdir`)
- Copying, moving, deleting files and directories (`cp`, `mv`, `rm`)
- Using a terminal text editor (`nano`)
  - `^X` means pressing `CTRL + x`
- Getting help
  - `man` to view the manual (man) page of a command
  - `help` to get information on shell built-ins (e.g. `help cd`).
    Try this if there is no man page available.
  - `info` to view the info page of a command.
    This can sometimes give you more information than the man page.
  - `apropos` to search for man pages
  - Appending `--help` to a command (e.g. `ls --help`)

Next, read the following man pages to get a better understanding of your surroundings:

- Read `man intro`
- Skim over `man man-pages`, take note of the sections
- Skim over `man file-hierarchy`
- Figure out the difference between `man printf` and `man 3 printf`

Finally:

- Figure out a way to copy files from ZID-GPL to your local system and vice versa.

Put together a summary of all the commands and man pages mentioned above.

- Write the summary on ZID-GPL using `nano`

## Task 3

This task is about working with permissions.

The provided archive [`task3.tar`](task3/task3.tar) contains two files. One of them is an executable called `binary` and the other a text file, `file.txt`. However, the file permissions were somehow mixed up, hence you are not able to run `binary`.

Please use the following command to unpack the archive:

```sh
tar -xpf task3.tar
```

What are the current permissions of the files, and what is in your opinion wrong about them?

Next, change the file permissions, so you can run `binary`. If you have done everything correctly, the content of `file.txt` should be printed by the executable. `binary` has been compiled to run on ZID-GPL (while you can also try to run it on your own computer, your mileage may vary).
Explain how you solved the problem.

When you list the contents of a directory with details you can see the permissions, owning user, and owning group among other details. For example:

```sh
$ ls -l
drwxrwxr-x. 2 c7012345 lxusers 4096 Mar 9 19:11 task_1_queue
-rw-rw-r--. 1 c7012345 lxusers 1088 Mar 9 04:20 task_2_summary.txt
-rw-rw-r--. 1 c7012345 lxusers   61 Mar 9 00:07 task_3_summary.txt

 ↑            ↑        ↑
 Permissions  User     Group
```

- Explain each of the permissions read, write, and execute for both files and directories.
- Explain how to read these permission strings (e.g. `rw-rw-r--`).
- How do these permission strings relate to octal permissions (e.g. `0644`)?
- In your opinion, what are the ideal permissions for ...
  - ... a text file used to take private notes?
  - ... a shell script?
  - ... a config file that should not be edited?

*Hint:* Take a look at `man chmod`.

Put your findings and used commands in a summary file.

--------------

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```
exc01_csXXXXXX.zip
├── group.txt            # optional
├── task_1_queue/
│   ├── Makefile         # optional
│   ├── myqueue_test.c         
│   └── myqueue.h
├── task_2_summary.txt
└── task_3_summary.txt
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
