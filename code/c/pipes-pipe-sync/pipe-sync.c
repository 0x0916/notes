#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void errExit(char *err) {
	perror(err);
	exit(EXIT_FAILURE);
}

char *currTime(const char *format) {
	static char buf[1000];
	time_t	t;
	size_t	s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, 1000, (format != NULL) ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}

int main(int argc, char **argv) {
	// process synchronization pipe
	int pipefd[2];
	int j, dummy;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		printf("%s sleep-time...\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	printf("%s Parent started\n", currTime("%T"));

	if (pipe(pipefd) == -1)
		errExit("pipe");

	for (j = 1; j < argc; j++) {
		switch (fork()) {
		case -1:
			errExit("fork");
		case 0:	// Child
			if (close(pipefd[0]) == -1)	// Read end is unused
				errExit("close");

			// Child does some work, and lets parent know it's done
			sleep(strtol(argv[j], NULL, 10));

			printf("%s child %d (PID=%ld) closing pipe\n",
				currTime("%T"), j, (long)getpid());
			if (close(pipefd[1]) == -1)
				errExit("close");

			// Child now carries on to do other things

			exit(EXIT_SUCCESS);
		default:	// Parent loops to create next child
			break;
		}
	}

	// Parent comes here; close wirte end of pipe so we can see EOF
	if (close(pipefd[1]) == -1)
		errExit("close");

	// Parent may do some other work, then synchronizes with children
	if (read(pipefd[0], &dummy, 1) != 0) {
		printf("parent didn't get EOF");
	}
	printf("%s Parent ready to go\n", currTime("%T"));

	// Parent can now carray on to do other things...
	exit(EXIT_SUCCESS);
}
