# Exercise Sheet 8

## Task 1

Programs frequently use _system calls_ to interact with the operating system, for example when spawning a child process or when performing everyday file operations.
It sometimes can be useful to inspect the system calls performed by a program, e.g. to debug its behavior when no source code is available, or simply to find out more about how it works.
Linux systems provide the `strace` utility to do just that.

Alongside this document you will find a binary file called [`secret`](secret), which has been compiled to run on ZID-GPL. You need to run the binary on ZID-GPL.

Begin by reading `man strace` to familiarize yourself with its output format.
Then, use `strace` to investigate the behavior of the provided binary. What is the binary trying to do? Try to use the binary as intended and report your findings.

## Task 2

In this task we will implement a simple "echo server" using TCP sockets.

Create a program called `server`, that receives as its single argument a TCP port to listen on.
After startup, the server opens a TCP socket (see below), prints `"Listening on port <port>."` and continuously listens for incoming connections.
Upon a client connecting, the server attempts to read incoming messages from the client and responds (using the established socket) with `"Echo: <original message>"`, either until the client disconnects or the shutdown command (see below) is received.
The server must not print the clients message the command line.

For this task, you don't need to create a separate client application.
Instead, you can use the `netcat` (`nc`) utility to establish a TCP connection to your server.
The command `nc localhost <port>` will open a prompt where you can send messages to your server and see its response.
You can exit `netcat` by pressing `CTRL+C`.

The server should be able to handle multiple clients connecting, but only one at a time (i.e., the current connection has to be closed before a new one can be accepted). Using multiple threads or processes is therefore not required.

Upon receiving the message `/shutdown`, the server should print `"Shutting down."`, stop accepting new connections, clean up and exit.

Begin by skimming through `man 7 ip` and `man 7 socket`. Here is a quick TLDR:

1. To create a TCP/IP socket, use the `socket()` function with `domain = AF_INET` and `type = SOCK_STREAM`.
2. Next, fill out the `sockaddr_in` struct to inform the socket where to bind to, like so:

   ```c
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(struct sockaddr_in));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   ```

3. Next, `bind()` the socket to the address, and `listen()` for incoming connections.
4. Use `accept()` to accept incoming connections, and to receive a unique communication socket for each client.

**Additional notes and hints:**

- The `dprintf` function may be useful for writing to a socket.
- Make sure to properly `close()` all sockets before exiting.
- `bind()` returning `EADDRINUSE` ("Address already in use"), may indicate that you did not properly close a socket.
  You can use a different port or find a way to circumvent this for testing.
  Ultimately you should however try to find the root cause of the problem.
- You **must not** use any global variables.
  However, you are allowed to `#define` preprocessor constants.
- Use `-D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE` to compile on ZID-GPL.

Example output of the server:

```text
Listening on port 1169.
Established connection!
Shutting down.
Closing connection ...
Connection closed.
```

Example output of `nc`:

```text
$ nc localhost 1169
Hello!
Echo: Hello!
^C
```

Answer the following questions:

- Why is it important to use network byte order with TCP/IP?
- The example uses `INADDR_ANY`. Could we also use `INADDR_LOOPBACK`?
- The example uses `SOCK_STREAM`. Name two other types of sockets and briefly explain their difference compared to `SOCK_STREAM`.
- What is the range of possible ports that we assign to `addr.sin_port`.
- Why is it a good idea to use a port greater or equal to 1024?

## Task 3

In this task we will turn the simple _echo server_ from the previous task into a simple _HTTP web server_.

- The server again receives two arguments:
    - `port` - the port it should listen on
    - `N` - the number of _request handlers_
- The server creates a queue for client connections. You may use the queue implementation from exercise sheet 06. 
- The server spawns `N` _request handler_ threads containing a loop:
  - Pop a client connection from the queue.
    - If the popped connection is a _poison value_ (e.g. `-1`), return.
    - Otherwise, receive and “parse” the HTTP request. You only need the first line of the request for this (see below).
  - To simulate a real workload, sleep for 100 milliseconds.
  - Send a response to the client:
    - When a `GET /` request is received, sends a response with a small HTML response body of your own choosing.
      Be sure to set the correct size in the `Content-Length` header field. (see below)
    - When a `POST /shutdown` request is received, the _listener_ thread is terminated.
      Since `accept` is blocking, one way of implementing this is to use the `pthread_cancel(3)` function.
    - Otherwise, you can either send the same response as for `GET /` or a custom error response (e.g. with HTTP status code `501 Not Implemented`).
  - Close the connection.
- The server creates and binds the socket as in the previous task, and then spawns a _listener_ thread for accepting incoming connections:
  - Each accepted connection is added to the client connection queue.
- The server waits for the _listener_ thread to finish.
- The server pushes `N` _poison values_ into the queue.
- The server waits for all _request handler_ threads to finish, cleans up and exits.

**Alternative implementation:** Instead of using a queue and `N` _request handler_ threads that handle requests in a loop, you can use your thread pool (with size `N`) from the previous exercise sheet. The _listener_ thread then submits a _request handler_ job to the thread pool for every client connection. You will need to add a variant of `pool_submit` which does not return a `job_id` since there is no good place to call `pool_await`.

If everything works correctly, you should be able to navigate your browser to `http://localhost:<port>/` and see your web server's response.
To shut it down, you can use `curl -X POST http://localhost:<port>/shutdown`.

### The HTTP Protocol

To properly communicate with a web browser, the web server needs to be able to “speak” the _HTTP protocol_, or at least a very simple subset thereof.
Most HTTP messages (and all that we care about here) consist of at most two parts, a _header section_ and optionally a _response body_.
The _header section_ contains multiple _header fields_ with meta-information about the current request or response.
Importantly, the HTTP protocol uses CRLF (`\r\n`) for line endings instead of just LF (`\n`).
A final empty newline marks the end of the header section.

When sending a `GET` request by opening `http://localhost:<port>/` in a browser or using `curl http://localhost:<port>/`, the server will receive a request that looks something like this:

```http
GET / HTTP/1.1\r\n
Host: localhost:<port>\r\n
Accept: */*\r\n
\r\n
```

When sending a `POST` using `curl -X POST http://localhost:<port>/shutdown`, the server receives a request like

```http
POST /shutdown HTTP/1.1\r\n
Host: localhost:<port>\r\n
Accept: */*\r\n
\r\n
```

To respond to a client, the server needs to write a header section and a response body, which typically looks like

```http
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: <number of bytes in response body, including newlines>\r\n
\r\n
<response body>
```

Example output:

```text
┌──────────────────────────────────┬───────────────────────────────────────────────┬───────────────────────────────┐
│ TERMINAL 1                       │ TERMINAL 2                                    │ TERMINAL 3                    │
├──────────────────────────────────┼───────────────────────────────────────────────┼───────────────────────────────┤
│ $ ./server 8888 5                │                                               │                               │
│ Listening on port 8888.          │                                               │                               │
│                                  │ $ curl http://localhost:8888/                 │ $ curl http://localhost:8888/ │
│                                  │ <response body>                               │ <response body>               │
│                                  │                                               │                               │
│                                  │                                               │                               │
│                                  │ $ curl -X POST http://localhost:8888/shutdown │ $ curl http://localhost:8888/ │
│ Shutting down.                   │                                               │ <response body>               │
│                                  │                                               │                               │
└──────────────────────────────────┴───────────────────────────────────────────────┴───────────────────────────────┘
```

**Additional notes and hints:**

- Use `curl -v http://localhost:<port>/` to see the request and response headers sent to and from your server.
  `curl` will complain if the response is malformed.
- For more information on the HTTP protocol, you may browse through [RFC 7230](https://datatracker.ietf.org/doc/html/rfc7230).
- When working on ZID-GPL, you can still connect to your web server with your local web browser by creating an _SSH tunnel_ using `ssh csXXXX@zid-gpl.uibk.ac.at -N -L <port>:localhost:<port>` where `<port>` refers to the port your server is listening on (make sure to choose a unique port to avoid collisions with other students).
- Use `-D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -D_BSD_SOURCE` on ZID

---

Submit your solution as a zip archive via OLAT, structured as follows, where csXXXXXX is your UIBK login name. Your zip archive **must not** contain binaries.

```text
exc08_csXXXXXX.zip
├── Makefile             # optional
├── group.txt            # optional
├── task1
│   └── solution.txt
├── task2
│   ├── Makefile
│   ├── server.c
│   └── solution.txt
└── task3
    ├── Makefile
    ├── client_queue.h   # optional if using thread pool
    ├── server.c
    ├── thread_pool.c    # optional if using client queue
    └── thread_pool.h    # optional if using client queue
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
