#include <string.h>
#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

// Invoked on exit to delete client FIFO
static void removeFifo(void) {
	unlink(clientFifo);
}

int main(int argc, char **argv) {
	int serverFd, clientFd;
	struct request req;
	struct response resp;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		fprintf(stdout, "%s [seq-len...]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Create our FIFO (before sending request, to avoid a race)

	umask(0);
	snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
		(long)getpid());

	if (mkfifo(clientFifo,S_IRUSR | S_IWUSR | S_IWGRP) == -1
			&& errno != EEXIST)
		errExit("mkfifo");

	if (atexit(removeFifo) != 0)
		errExit("atexit");


	// construct request message, open server FIFO, and send request
	req.pid = getpid();
	req.seqLen = (argc > 1) ? strtol(argv[1], NULL, 10) : 1;

	serverFd = open(SERVER_FIFO, O_WRONLY);
	if (serverFd == -1)
		errExit("open");

	if (write(serverFd, &req, sizeof(struct request)) !=
			sizeof(struct request)) {
		errExit("can not write to server");
	}

	// Open our FIFO, read and display response
	clientFd = open(clientFifo, O_RDONLY);
	if (clientFd == -1)
		errExit("open");

	if (read(clientFd, &resp, sizeof(struct response)) !=
			sizeof(struct response)) {
		errExit("can not read form server");
	}

	printf("%d\n", resp.seqNum);;
	exit(EXIT_SUCCESS);
}
