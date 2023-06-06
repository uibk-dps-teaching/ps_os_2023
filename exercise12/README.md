# Exercise Sheet 12

The following is a _real_ exam from a previous year.
It should give you a general idea of what to expect from this year's exam.

This exercise sheet is optional, and provides up to 3 **bonus points**:
These points are not included in the total number of points reachable this semester (33), but will count towards your personal final score (theoretically, it is thus possible to achieve 36/33 points).

While the exam itself is not split into explicit tasks, we define the following tasks for receiving the bonus points:

- **Task 1:** Implement the functionality to set the temperature (client gets no response).
- **Task 2:** Implement the functionality to get the temperature (client gets response).
- **Task 3:** Implement the functionality to calculate the average temperature.

**As usual, please mark your completed tasks in OLAT**.

**NOTE**: We recommend that you attempt to solve this exercise sheet by simulating the real exam scenario:
Prepare any materials you may need, such as your solutions to previous exercises, for quick access (the real exam will be open book!).
Take an hour and 45 minutes and only then continue reading the rest of this sheet.
Try to solve it in time.

---

# Exam

## General Information

**Please read this description carefully.**

Talking, chatting, texting, or any other form of a live human to human communication is not allowed and may lead to a negative grade.
However, you are allowed to use all other offline and online resources such as previous homework solutions, manpages, StackOverflow, etc.

If you directly copy code from one of these sources, please add a **citation** as a comment before the code fragment. i.e.

```C
// The following code is from my solution to exercise 5.2
```

or

```C
// Source: https://stackoverflow.com/questions/14888027/mutex-lock-threads
```

If we find the same code in multiple solutions without citation, we have to assume cheating, and you will receive a failing grade.
Note that it does not matter whether you are the person copying from, or providing your solution to another -- both will receive a failing grade.

Your program **must compile** on ZID-GPL (or ZID-Desk) and include a Makefile.
If something does not work as expected, provide comments in the source code explaining what you think the problem is.
It might improve your grade.

**You have 1 hour and 45 minutes to complete this exercise.**

**Please make sure to upload your solution to OLAT in time.**

### General Hints

- Don't forget you can load a newer version of GCC on ZID-GPL using e.g. `module load gcc/9.2.0`.
- Remember that you can use `valgrind` to check your program for memory leaks.
- Try to develop your program incrementally, and focus on the core functionality first.

\newpage

# Task: Temperature Monitor

You would like to monitor the temperature around your house, with multiple sensors (one in each room) reporting their current readings to a centralized server.
The server can then be queried for the current temperature in a given room, as well as the average temperature across the house.
Your solution should consist of two executables: `temp_client` and `temp_server`.
The client can be used to both set temperatures as well as query the current temperature for a given sensor from the server.
Use TCP sockets to communicate between client and server.

**Client:**

- The client receives four arguments: `./temp_client <port> <command> <sensor-id> <temperature>`
- The only `<command>` supported (initially, see below) is
  - `set` followed by a `sensor-id` (a string) and `temperature` (a floating point number), which will then set the current temperature for a given sensor (for example `./temp_client 3000 set bedroom 22.3`).
- The client then opens a TCP connection to the server on the given port, sends the command along with its arguments and then exits.

**Server:**

- The server receives as a single argument the port it should listen on: `./temp_server <port>`.
- It then opens a TCP socket, listens for incoming connections and prints `"Listening on port <port>."`.
- Communication with the server uses a simple text-based protocol (i.e., it should be possible to communicate with your server using `telnet`).
- For each incoming connection, the server spawns a request handler thread, i.e., it should be able to handle multiple client requests concurrently.
- When receiving the request `set <sensor-id> <temperature>`, the server stores the `temperature` for the given `sensor-id` and prints `"Setting temperature for <sensor-id>: <temperature> °C."`.
- **For simplicity** you can assume that the server always handles **exactly 5 requests** before shutting down.

Once you have implemented this functionality, extend your client and server to support two additional commands:

- `get <sensor-id>` retrieves the current temperature for a given sensor, with the client printing `"The <sensor-id> temperature is <temperature> °C."` or `"No temperature found for <sensor-id>."` if no temperature has been set.
- `average` retrieves the average temperature across all sensors, with the client printing `"The temperature averages <average> °C."`.
  To be able to test the concurrency of your server, **simulate some workload by sleeping 5 seconds** before computing and returning the average.

**Example output:**

\scriptsize

```
TERMINAL 1                                  TERMINAL 2                                  TERMINAL 3

$ ./temp_server 9999
Listening on port 9999.
                                            $ ./temp_client 9999 set bedroom 21.3
Setting temperature for bedroom: 21.3 °C.
                                            $ ./temp_client 9999 get kitchen
                                              No temperature found for kitchen.
                                                                                        $ ./temp_client 9999 average
                                            $ ./temp_client 9999 set kitchen 24.5
Setting temperature for kitchen: 24.5 °C.
                                            $ ./temp_client 9999 get bedroom
                                              The bedroom temperature is 21.3 °C.

                                                                                         The temperature averages 22.9 °C.
Shutting down.
```

\normalsize

**Additional notes and hints:**

- You may assume a reasonable maximum length for sensor ids, requests and response messages.
- Use `sprintf` / `sscanf` to conveniently construct / parse requests on the client / server.

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc12_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # we recommend to work alone for this exercise sheet
├── task1
│   ├── Makefile
│   ├── client.c
│   ├── server.c
│   └── shared.h         # optional
├── task2
│   ├── Makefile
│   ├── client.c
│   ├── server.c
│   └── shared.h         # optional
└── task3
    ├── Makefile
    ├── client.c
    ├── server.c
    └── shared.h         # optional
```

Requirements

- [ ] Auto-format all source files
- [ ] Check your submission on ZID-GPL
- [ ] Check your file structure
- [ ] Submit zip
- [ ] Mark solved exercises in OLAT
- [ ] Any implementation MUST NOT produce any additional output