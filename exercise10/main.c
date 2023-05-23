#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <bench|test>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strncmp(argv[1], "bench", 5) == 0) {
		run_bench();
		return EXIT_SUCCESS;
	}

	if (strncmp(argv[1], "test", 4) == 0) {
		run_tests();
		return EXIT_SUCCESS;
	}
}
