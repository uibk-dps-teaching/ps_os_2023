CPPFLAGS += -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -D_BSD_SOURCE
CFLAGS += -std=c11 -Wall -Wextra -pedantic -pthread -g -O3
LDFLAGS += -pthread

# Add the names of your executables here
TARGETS = dummy_allocator

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o

membench.o: membench.h

allocator_tests.o: allocator_tests.h

main.o: allocator.h

dummy_allocator.o: allocator.h allocator_tests.h membench.h

dummy_allocator: membench.o allocator_tests.o main.o

# Add rules similar to dummy_allocator to build your code.
# The only change which should be required is changing the name of the rule,
# the first prerequisite, and adding the rule to TARGETS (space separated)
# You can implement the global and thread local versions of the same allocator in the same source file
# and use the preprocessor to build two different executables from the same source.
