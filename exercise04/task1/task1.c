#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void* string_manipulation(void* arg) {
	char* input = (char*)arg;
	size_t input_length = strlen(input);

	char reversed_input[input_length];
	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		reversed_input[input_length - char_index - 1] = input[char_index];
	}

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		input[char_index] = toupper(input[char_index]);
	}

	return reversed_input;
}

int main(int argc, const char** argv) {
	char* shared_strings[argc];

	for (int arg_index = 0; arg_index < argc; ++arg_index) {
		size_t arg_length = strlen(argv[arg_index]);
		shared_strings[arg_index] = malloc(arg_length * sizeof(char));
		if (shared_strings[arg_index] == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		strcpy(shared_strings[arg_index], argv[arg_index]);
	}

	pthread_t t_ids[argc];
	for (int arg_index = 0; arg_index <= argc; arg_index++) {
		pthread_create(&t_ids[arg_index], NULL, string_manipulation, shared_strings[arg_index]);
		free(shared_strings[arg_index]);
	}

	for (int arg_index = 0; arg_index <= argc; arg_index++) {
		char* reversed_string;
		waitpid(*t_ids + arg_index, (void*)&reversed_string, 0);
		printf("original argv[%d]: %s\nuppercased: %s\nreversed: %s\n", arg_index, argv[arg_index],
		       shared_strings[arg_index], reversed_string);
	}

	return EXIT_SUCCESS;
}
