#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>

#include <sys/types.h>

int main(int argc, char **argv) {
	int num;
	int n,i;
	gid_t *grouplist, *newgrouplist;
	gid_t list[16];

	// get the total number of supplementary group IDs
	num = getgroups(0, NULL);
	if (num < 0) {
		perror("getgroups error");
		return 1;
	}
	printf("the total number of groups: %d\n", num);

	grouplist = (gid_t*)malloc(sizeof(gid_t)*num);
	// get list of supplementary group IDs
	n = getgroups(num, grouplist);
	for (i = 0; i < n; i++) {
		printf("(%d) group ID: %d\n", i, grouplist[i]);
	}

	newgrouplist = (gid_t*)malloc(sizeof(gid_t)*(n+1));
	for (i = 0; i < n; i++) {
		newgrouplist[i] = grouplist[i];
	}
	newgrouplist[n] = 1000;
	// set the supplementary group IDs for the calling process
	if (setgroups(n+1, newgrouplist) < 0) {
		perror("setgroups error");
	}

	// get list of supplementary group IDs
	printf("after getgroups\n");
	n = getgroups(16, list);
	for (i = 0; i < n; i++) {
		printf("(%d) group ID: %d\n", i, list[i]);
	}

	free(grouplist);
	free(newgrouplist);
	return 0;
}
