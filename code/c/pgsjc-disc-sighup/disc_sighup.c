#define _GNU_SOURCE  // get strsignal() declaration from <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Handler for SIGHUP
static void handler(int sig)
{
	printf("PID %ld: caught signal %2d (%s)\n", (long)getpid(),
		sig, strsignal(sig));
}

int main(int argc, char **argv) {
	pid_t	childPid, parentPid;
	int j;
	struct sigaction sa;

	setbuf(stdout, NULL); // make stdout unbuffered

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		printf("%s {d|s}... [> sig.log 2>&1 ]\n", argv[0]);
		exit(0);
	}

	parentPid = getpid();
	printf("PID of parent process is :	%ld\n", (long)parentPid);
	printf("Foreground process group ID is: %ld\n", 
		(long)tcgetpgrp(STDIN_FILENO));

	// Create child process
	for (j = 1; j < argc; j++) {
		childPid = fork();
		if (childPid == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (childPid == 0){	// child
			if (argv[j][0] == 'd')
				if (setpgid(0,0) == -1) {
					perror("setpgid");
					exit(EXIT_FAILURE);
				}
			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = handler;
			if (sigaction(SIGHUP, &sa, NULL) == -1) {
				perror("sigaction");
				exit(EXIT_FAILURE);
			}
			break;	// child exits loop
		}
	}

	// All processes fall throuth to here

	// An unhandled SIGALRM ensure this process will die if nothing else terminates it
	alarm(60);

	printf("PID=%ld, PPID=%ld\n", (long)getpid(), (long)getppid());

	// wait for signals
	for(;;) {
		pause();
	}
}
