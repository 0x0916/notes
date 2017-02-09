#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void handler(int sig)
{}

int main(int argc, char **argv) {
	pid_t	childPid;
	struct sigaction sa;

	setbuf(stdout, NULL); // make stdout unbuffered

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	childPid = fork();
	if (childPid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (childPid == 0 && argc > 1)
		// Move to new process group
		if (setpgid(0,0) == -1) {
			perror("setpgid");
			exit(EXIT_FAILURE);
		}

	printf("PID=%ld, PPID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(),
		(long)getppid(), (long)getpgid(0), (long)getsid(0));

	// An unhandled SIGALRM ensure this process will die if nothing else terminates it
	alarm(60);

	// wait for signals
	for(;;) {
		pause();
		printf("%ld: caught signal SIGHUP\n", (long)getpid());
	}
}
