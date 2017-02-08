#define	_XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (fork() != 0)	// Exit if parent, or on error
		exit(EXIT_SUCCESS);

	// child process
	if (setsid() == -1) {
		perror("setsid");
		exit(EXIT_FAILURE);
	}


	printf("PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(),
		(long)getpgid(0), (long)getsid(0));

	if (open("/dev/tty", O_RDWR)== -1) {
		perror("open /dev/tty");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
