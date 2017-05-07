#include <stdio.h>
#include <seccomp.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	const struct scmp_version *version;

	version = seccomp_version();

	if (!version)
		goto err;

	printf("libsecomp version: %d.%d.%d\n",
		version->major,
		version->minor,
		version->micro);
	exit(EXIT_SUCCESS);
err:
	exit(EXIT_FAILURE);
}
