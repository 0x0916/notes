#include <sys/types.h>
#include <sys/xattr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	ssize_t buflen, keylen, vallen;
	char *buf, *key, *val;

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
		printf("%s: ", key);
		// Determine length of the value
		vallen = getxattr(argv[1], key, NULL, 0);
		if (vallen == -1)
			perror("getxattr");
		if (vallen > 0) {
			// allocate value buf
			val = malloc(vallen + 1);
			if (val == NULL) {
				perror("malloc");
				free(buf);
				exit(EXIT_FAILURE);
			}
			// copy value to buf
			vallen = getxattr(argv[1], key, val, vallen);
			if (vallen == -1)
				perror("getxattr");
			else {
				val[vallen] = '\0';
				printf("%s", val);
			}
			free(val);
		}else if (vallen == 0)
			printf("<no value>");

		printf("\n");
		keylen = strlen(key) + 1;
		buflen -= keylen;
		key += keylen;
	}

	free(buf);
	exit(EXIT_SUCCESS);
}

/*
# gcc -o getxattr getxattr.c 
# touch /tmp/foo
# setfattr -n user.fred -v chocolate /tmp/foo
# setfattr -n user.frieda -v bar /tmp/foo
# setfattr -n user.empty  /tmp/foo
# ./getxattr /tmp/foo
user.fred: chocolate
user.frieda: bar
user.empty: <no value>
*/
