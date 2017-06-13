#include <sys/types.h>
#include <sys/xattr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	ssize_t buflen, keylen;
	char *buf, *key;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s path\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Determine the length of the buffer needed.
	buflen = listxattr(argv[1], NULL, 0);
	if (buflen == -1) {
		perror("listxattr");
		exit(EXIT_FAILURE);
	}
	if (buflen == 0) {
		printf("%s has no attributes.\n", argv[1]);
		exit(EXIT_SUCCESS);
	}

	// Allocate the buffer
	buf = malloc(buflen);
	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// Copy the list of attribute keys to the buffer
	buflen = listxattr(argv[1], buf, buflen);
	if (buflen == -1) {
		perror("listxattr");
		free(buf);
		exit(EXIT_FAILURE);
	}

	// Loop over the list of zero terminated strings with
	// the attribute keys.
	key = buf;
	while (buflen > 0) {
		printf("KEY: %s\n", key);
		keylen = strlen(key) + 1;
		buflen -= keylen;
		key += keylen;
	}

	free(buf);
	exit(EXIT_SUCCESS);
}

/*
# gcc -o listxattr listxattr.c 
# touch /tmp/foo
# setfattr -n user.fred -v chocolate /tmp/foo
# setfattr -n user.frieda -v bar /tmp/foo
# setfattr -n user.empty  /tmp/foo
# ./listxattr /tmp/foo 
KEY: user.fred
KEY: user.frieda
KEY: user.empty
*/
