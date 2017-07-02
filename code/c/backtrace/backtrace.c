#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BT_BUF_SIZE 100

void func3(void) {
	int size, i;
	void *buffer[BT_BUF_SIZE];
	char **strings;

	size = backtrace(buffer, BT_BUF_SIZE);
	printf("backtrace returned %d addresses\n", size);

	strings = backtrace_symbols(buffer, size);
	if (strings == NULL) {
		perror("backtrace_symbol");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < size; i++)
		printf("%s\n", strings[i]);

	free(strings);
}

// "static" means don't export the symbol...
static void func2(void) {
	func3();
}

void func(int ncalls) {
	if (ncalls > 1)
		func(ncalls - 1);
	else
		func2();
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "%s num-calls\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	func(atoi(argv[1]));
	exit(EXIT_SUCCESS);
}
