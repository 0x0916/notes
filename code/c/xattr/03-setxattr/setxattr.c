#include <sys/types.h>
#include <sys/xattr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int list_xattr(const char *pathname);

int main(int argc, char **argv) {
	int ret;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s path\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Get the xattr of file
	printf("## File attr\n");
	ret = list_xattr(argv[1]);
	if (ret < 0) {
		perror("list_xattr");
		exit(EXIT_FAILURE);
	}

	// Set the xattr of file
	printf("## Setting File attr...\n");
	printf("## Setting attr user.joe = apple\n");
	printf("## Setting attr user.jim = banana\n");
	ret = setxattr(argv[1], "user.joe", (void *)"apple", 6, 0);
	if (ret < 0) {
		perror("setxattr");
		exit(EXIT_FAILURE);
	}
	ret = setxattr(argv[1], "user.jim", (void *)"banana", 7, 0);
	if (ret < 0) {
		perror("setxattr");
		exit(EXIT_FAILURE);
	}

	// Get the xattr of file
	printf("## File attr(after setting):\n");
	ret = list_xattr(argv[1]);
	if (ret < 0) {
		perror("list_xattr");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

int list_xattr(const char *pathname) {
	ssize_t buflen, keylen, vallen;
	char *buf, *key, *val;

	// Determine the length of the buffer needed.
	buflen = listxattr(pathname, NULL, 0);
	if (buflen == -1) {
		perror("listxattr");
		return -1;
	}
	if (buflen == 0) {
		printf("%s has no attributes.\n", pathname);
		return 0;
	}

	// Allocate the buffer
	buf = malloc(buflen);
	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// Copy the list of attribute keys to the buffer
	buflen = listxattr(pathname, buf, buflen);
	if (buflen == -1) {
		perror("listxattr");
		free(buf);
		return 0;
	}

	// Loop over the list of zero terminated strings with
	// the attribute keys.
	key = buf;
	while (buflen > 0) {
		printf("%s: ", key);
		// Determine length of the value
		vallen = getxattr(pathname, key, NULL, 0);
		if (vallen == -1)
			perror("getxattr");
		if (vallen > 0) {
			// allocate value buf
			val = malloc(vallen + 1);
			if (val == NULL) {
				perror("malloc");
				free(buf);
				return -1;
			}
			// copy value to buf
			vallen = getxattr(pathname, key, val, vallen);
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
	return 0;
}

/*
# gcc setxattr.c -o setxattr
# ./setxattr /tmp/foo 
## File attr
user.fred: chocolate
user.frieda: bar
user.empty: <no value>
## Setting File attr...
## Setting attr user.joe = apple
## Setting attr user.jim = banana
## File attr(after setting):
user.fred: chocolate
user.frieda: bar
user.empty: <no value>
user.jim: banana
user.joe: apple
*/
