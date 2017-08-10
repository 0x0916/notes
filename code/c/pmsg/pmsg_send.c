#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

static void usageError(const char *progname) {
	fprintf(stderr, "Usage: %s [-n] name msg [prio]\n", progname);
	fprintf(stderr, "	-n	Use O_NONBLOCK flag\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	int flags, opt;
	mqd_t	mqd;
	unsigned int prio;

	flags = O_WRONLY;
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

	prio = (argc > optind + 2) ? atoi(argv[optind + 2]) : 0;
	if (mq_send(mqd, argv[optind+1], strlen(argv[optind+1]), prio) == -1) {
		fprintf(stderr, "error: mq_send\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
