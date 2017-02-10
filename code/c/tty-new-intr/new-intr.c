#include <termios.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	struct termios tp;
	int intr_char;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		printf("%s [intr-char]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Determine new INTR setting from command line
	if (argc == 1) {
		intr_char = fpathconf(STDIN_FILENO, _PC_VDISABLE);
		if (intr_char == -1) {
			perror("Could not determine VDISABLE");
			exit(EXIT_FAILURE);
		}
	}else if (isdigit((unsigned char )argv[1][0])) {
		intr_char = strtoul(argv[1], NULL, 0);
	}else {
		intr_char = argv[1][0];
	}

	// Fetch current terminal settings, modify INTR character, and push
	// changes back to the terminal driver
	if (tcgetattr(STDIN_FILENO, &tp) == -1) {
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}
	tp.c_cc[VINTR] = intr_char;
	if (tcsetattr(STDIN_FILENO,TCSAFLUSH,  &tp) == -1) {
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
