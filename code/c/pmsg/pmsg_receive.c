#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

static void usageError(const char *progname) {
	fprintf(stderr, "Usage: %s [-n] name\n", progname);
	fprintf(stderr, "	-n	Use O_NONBLOCK flag\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	int flags, opt;
	mqd_t	mqd;
	unsigned int prio;
	void *buffer;
	struct mq_attr attr;
	ssize_t numRead;

	flags = O_RDONLY;
	while((opt = getopt(argc, argv, "n")) != -1) {
		switch(opt) {
		case 'n':
			flags |= O_NONBLOCK;
			break;
		default:
			usageError(argv[0]);
			break;
		}
	}

	if (optind + 1 > argc)
		usageError(argv[0]);

	mqd = mq_open(argv[optind], flags);
	if (mqd == (mqd_t)-1) {
		fprintf(stderr, "error: mq_open\n");
		exit(EXIT_FAILURE);
	}

	if (mq_getattr(mqd, &attr) == -1) {
		fprintf(stderr, "error: mq_getattr\n");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (buffer == NULL) {
		fprintf(stderr, "error: malloc\n");
		exit(EXIT_FAILURE);
	}

	numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
	if (numRead == -1) {
		fprintf(stderr, "error: mq_receive\n");
		exit(EXIT_FAILURE);
	}

	printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
	if (write(STDOUT_FILENO, buffer, numRead) == -1) {
		fprintf(stderr, "error: write\n");
		exit(EXIT_FAILURE);
	}

	printf("\n");
	exit(EXIT_SUCCESS);
}
