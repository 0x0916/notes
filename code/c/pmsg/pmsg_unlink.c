#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		printf("%s: mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (mq_unlink(argv[1]) == -1) {
		printf("error: mq_unlink");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
