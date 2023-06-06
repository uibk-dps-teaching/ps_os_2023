# UIBK Operating Systems Lab 2023

This repository contains material required to complete exercises for the OS lab in the 2023 summer semester, including exercise sheets and associated resources.

**Make sure to read this document carefully, as it contains vital information regarding the grading of exercises.**

## Schedule

This schedule gives you an overview of the topics we will discuss on each date. It will be updated continuously throughout the semester.
Note that exercises associated with a given date are to be discussed on that day, i.e., solutions for that exercise have to be handed in via OLAT by 17:00 on the previous day (Tuesdays).

| Date       | Exc              | Topic                                                 | Notes               |
| ---------- | ---------------- | -------------------------------------                 | ------------------- |
| 2023-03-08 | -                |                                                       | C recap slides    |
| 2023-03-15 | [1](exercise01/) | Self-assessment, ssh, permissions                     | Makefile slides   |
| 2023-03-22 | [2](exercise02/) | Makefile, signal, fork                                |                   |
| 2023-03-29 | [3](exercise03/) | Env vars, exit codes, pthreads                        |                   |
| 2023-04-05 | -                |                                                       | Break             |
| 2023-04-12 | -                |                                                       | Break             |
| 2023-04-19 | [4](exercise04/) | Debugging, pipes, fifos                               |                   |
| 2023-04-26 | [5](exercise05/) | Shared memory, synchronization, message queues        |                   |
| 2023-05-03 | [6](exercise06/) | Atomics, mutex, condition variables                   |                   |
| 2023-05-10 | [7](exercise07/) | Barriers, mutex, thread pool                          |                   |
| 2023-05-17 | [8](exercise08/) | strace, TCP sockets, HTTP web server                  |                   |
| 2023-05-24 | [9](exercise09/) | TCP sockets (chat)                                    |                   |
| 2023-05-31 | [10](exercise10/)| Custom allocators                                     |                   |
| 2023-06-07 | [11](exercise11/)| Linking                                               |                   |
| 2023-06-14 | [12](exercise12/)| Last year's test                                      | Bonus points      |
| 2023-06-21 | -                |                                                       | Test              |
| 2023-06-28 | -                |                                                       | Q&A, Feedback     |

## Coding Guidelines

All programming exercises are conducted in C.

Your code should always compile without warnings, when passing the flags `-Wall -Wextra -std=c11`.

Make sure your code is properly formatted using either your IDE/Text editor of choice, or by using a tool such as clang-format.
You can find an example [`.clang-format`](.clang-format) file in this repository.
Failure to consistently format code may result in lower scores.

Make sure to choose descriptive variable and function names.
**All names and comments must be written in English.**

## Working From Home with ZID-GPL

All programming exercise solutions you hand in **must** compile properly on the [ZID-GPL server](https://www.uibk.ac.at/zid/systeme/linux/lpccs_4/benutzeranleitung_zid-gpl.html).
The easiest way of ensuring this is by developing on ZID-GPL directly, either through a terminal or using an editor/IDE with support for remote development via SSH (such as [Visual Studio Code](https://code.visualstudio.com/docs/remote/ssh)).

Note that the default version of the GCC compiler present on ZID-GPL is _extremely_ outdated.
You may load a newer version by calling `module load gcc/9.2.0`.

It appears as there is a bug where man pages cannot be viewed after loading a module.
This has been reported to ZID and be fixed (hopefully) soon.
In the meantime please use the following command to work around this issue:

`export MANPATH=":$MANPATH"`

## Connecting to ZID-GPL via SSH

To log into ZID-GPL via SSH, use the following command on Linux/Mac OSX:

`ssh -l <username> zid-gpl.uibk.ac.at`

On Windows, you can use Powershell (Windows 10) or a third-party SSH client
such as [PuTTY](https://www.putty.org/).
