#include <grp.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int n,i;

	if (argc < 2) {
		printf("Usage: %s <user>\n", argv[0]);
		return 0;
	}

	// initialize the group access list and using groups of which user is a memeber.
	if (initgroups(argv[1], 1234) < 0) {
		perror("initgroups");
		return 1;
	}

	gid_t grouplist[16];
	n = getgroups(16, grouplist);
	for (i = 0; i < n; i++) {
		printf("(%d) group ID: %d\n", i, grouplist[i]);
	}

	return 0;
}
