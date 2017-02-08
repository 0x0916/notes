#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


// Well-known name for server's FIFO
#define	SERVER_FIFO  "/tmp/seqnum_sv"
// Template for building client FIFO name
#define	CLIENT_FIFO_TEMPLATE	"/tmp/seqnum_cl.%ld"
// Space required for client FIFO pathname
#define	CLIENT_FIFO_NAME_LEN	(sizeof(CLIENT_FIFO_TEMPLATE) + 20)

// Request: client --> server
struct request {
	pid_t	pid;	// PID of client
	int seqLen;	// Length of desired sequence
};

// Response: server --> client
struct response {
	int	seqNum;	// Start of sequence
};

void errExit(const char *err) {
	perror(err);
	exit(EXIT_FAILURE);
}
