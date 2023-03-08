#include "myqueue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	myqueue queue;
	myqueue_init(&queue);

	assert(myqueue_is_empty(&queue));

	myqueue_enqueue(&queue, 1);

	assert(!myqueue_is_empty(&queue));

	assert(1 == myqueue_dequeue(&queue));

	assert(myqueue_is_empty(&queue));

	myqueue_enqueue(&queue, 1);
	myqueue_enqueue(&queue, 2);
	myqueue_enqueue(&queue, 3);
	myqueue_enqueue(&queue, 4);

	assert(1 == myqueue_dequeue(&queue));
	assert(2 == myqueue_dequeue(&queue));
	assert(3 == myqueue_dequeue(&queue));

	assert(!myqueue_is_empty(&queue));

	myqueue_destroy(&queue);

	puts("All good!");

	return EXIT_SUCCESS;
}
