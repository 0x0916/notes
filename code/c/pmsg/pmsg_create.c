#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static void usageError(const char *progname) {
	fprintf(stderr, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name [octal-perms]\n",
		progname);
	fprintf(stderr, "	-c		Create queue (O_CREAT)\n");
	fprintf(stderr, "	-x		Create exclusively(O_EXCL)\n");
	fprintf(stderr, "	-m maxmsg	Set maximum # of messages\n");
	fprintf(stderr, "	-s msgsize	Set maximum message size\n");
	exit(EXIT_FAILURE);
}

int getInt(const char *arg, int base) {
	long res;
	char *endptr;

	errno = 0;
	res = strtol(arg, &endptr, base);
	if (errno != 0) {
		fprintf(stderr, "strtol() failed\n");
		exit(EXIT_FAILURE);
	}

	if (*endptr != '\0') {
		fprintf(stderr, "non-numeric characters\n");
		exit(EXIT_FAILURE);
	}

	return (int)res;
}

int main(int argc, char **argv) {
	int flags, opt;
	mode_t perms;
	mqd_t mqd;
	struct mq_attr attr, *attrp;

	attrp = NULL;
	attr.mq_maxmsg = 50;
	attr.mq_msgsize = 2048;
	flags = O_RDWR;

	// Parse command-line options
	while ((opt = getopt(argc, argv, "cm:s:x")) != -1) {
		switch(opt) {
		case 'c':
			flags |= O_CREAT;
			break;
		case 'x':
			flags |= O_EXCL;
			break;
		case 'm':
			attr.mq_maxmsg = atoi(optarg);
			attrp = &attr;
			break;
		case 's':
			attr.mq_msgsize = atoi(optarg);
			attrp = &attr;
			break;
		default:
			usageError(argv[0]);
			break;
		}
	}

	if (optind >= argc)
		usageError(argv[0]);

	perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) :
			getInt(argv[optind+1], 8);
	mqd = mq_open(argv[optind], flags, perms, attrp);
	if (mqd == (mqd_t) -1) {
		fprintf(stderr, "error: mq_open\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
