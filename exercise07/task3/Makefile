CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -g -pthread
LDFLAGS = -pthread

TARGETS = task3a task3b

.PHONY:
all: $(TARGETS)

.PHONY:
clean:
	$(RM) $(TARGETS) *.o

task3a: task3.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DTHREAD_PER_JOB -o $@ $<

thread_pool.o: thread_pool.c thread_pool.h

task3b.o: task3.c thread_pool.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DTHREAD_POOL -c -o $@ $<

task3b: task3b.o thread_pool.o
