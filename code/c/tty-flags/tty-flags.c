#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
	struct termios ts, ots;
	int ret;
	tcgetattr(STDIN_FILENO, &ts);
	ots = ts;

//	ts.c_oflag |= ONLRET;
	//ts.c_oflag |= OCRNL;
	//ts.c_oflag |= ONOCR;
	cfmakeraw(&ts);
	ts.c_oflag |= ONLCR;
	ts.c_oflag |= OPOST;
	ret = tcsetattr(STDIN_FILENO, TCSANOW, &ts);
	if (ret < 0) {
		perror("111tcsetattr failed");
		printf("errno = %d\n", errno);
		exit(1);
	}

	tcgetattr(STDOUT_FILENO, &ts);
	if (ts.c_oflag & ONOCR) {
		fprintf(stderr, "222tcsetattr failed\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &ots);
		exit(1);
	}

	printf("hello world\n");
	printf("hello world\n");
	printf("hello world\n");
	printf("hello world\n");
	printf("hello world\n");

	tcsetattr(STDIN_FILENO, TCSANOW, &ots);

	exit(0);
}
