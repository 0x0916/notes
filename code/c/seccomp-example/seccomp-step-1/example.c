#define _GNU_SOURCE 1
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "config.h"


int main(int argc, char **argv) {
	char buf[1024];

	printf("Type stuff here: ");
	fflush(NULL);
	buf[0] = '\0';
	fgets(buf, sizeof(buf), stdin);
	printf("You typed: %s", buf);

	printf("And now we fork, which should do quite opposite...\n");
	fflush(NULL);
	sleep(1);

	fork();

	printf("You should not see this, because I am dead\n");

	return 0;
}
