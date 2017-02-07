#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 10

void errExit(char *err) {
	perror(err);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	// Pipe file descriptions
	int pipefd[2];
	char buf[BUF_SIZE];
	ssize_t	num;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		printf("%s string\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Create a pipe
	if (pipe(pipefd) == -1)
		errExit("pipe");

	switch (fork()) {
	case -1:
		errExit("fork");
	case 0:
		// Child - reads from pipe
		// Close the unused write end of pipe
		if (close(pipefd[1]) == -1)
			errExit("close - child");

		// Read data from pipe, echo on stdout
		for (;;) {
			num = read(pipefd[0], buf, BUF_SIZE);
			if (num == -1)
				errExit("read");
			if (num == 0)
				break;	// End-Of-File
			if (write(STDOUT_FILENO, buf, num) != num) {
				printf("child - partial/failed write\n");
				exit(EXIT_FAILURE);
			}
		}

		write(STDOUT_FILENO, "\n", 1);
		if (close(pipefd[0]) == -1)
			errExit("close");
		_exit(EXIT_SUCCESS);
	default:
		// Parent - writes to pipe
		// Close the unused read end of pipe
		if (close(pipefd[0]) == -1)
			errExit("close - parent");

		if (write(pipefd[1], argv[1], strlen(argv[1]))!= strlen(argv[1])){
			printf("parent - partial/failed write\n");
			exit(EXIT_FAILURE);
		}

		// child will see EOF
		if (close(pipefd[1]) == -1)
			errExit("close");
		wait(NULL);
		exit(EXIT_SUCCESS);
	}
}
