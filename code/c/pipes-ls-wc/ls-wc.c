#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void errExit(const char *err) {
	perror(err);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	int pipefd[2];

	if (pipe(pipefd) == -1)
		errExit("pipe");

	switch(fork()) {
	case -1:
		errExit("fork");
	case 0:	// First child: exec ls to write to pipe
		if (close(pipefd[0]) == -1)
			errExit("close 1");
		// Duplicate output on write end of pipe; close duplicated descriptor
		if (pipefd[1] != STDOUT_FILENO) {
			if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				errExit("dup2 1");
			if (close(pipefd[1]) == -1)
				errExit("close 2");
		}
		execlp("ls", "ls", (char *)NULL);	// Writes to pipe
		errExit("execlp ls");
	default:// Parent falls through to create next child
		break;
	}

	switch(fork()) {
	case -1:
		errExit("fork");
	case 0:	// Second child: exec "wc" to read to pipe
		if (close(pipefd[1]) == -1)
			errExit("close 3");
		// Duplicate output on read end of pipe; close duplicated descriptor
		if (pipefd[0] != STDIN_FILENO) {
			if (dup2(pipefd[0], STDIN_FILENO) == -1)
				errExit("dup2 2");
			if (close(pipefd[0]) == -1)
				errExit("close 4");
		}
		execlp("wc", "wc", (char *)NULL);	// Reads to pipe
		errExit("execlp wc");
	default:// Parent falls through
		break;
	}


	// Parent closes unused file descriptors for pipe, and waits for children
	if (close(pipefd[0]) == -1)
		errExit("close 5");
	if (close(pipefd[1]) == -1)
		errExit("close 6");
	if (wait(NULL) == -1)
		errExit("wait 1");
	if (wait(NULL) == -1)
		errExit("wait 2");

	exit(EXIT_SUCCESS);
}
