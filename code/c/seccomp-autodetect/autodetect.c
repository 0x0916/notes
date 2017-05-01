#include <stdio.h>
#include <errno.h>
#include <sys/prctl.h>
#include <string.h>
#include <linux/seccomp.h>

int main(int argc, char **argv) {
	int ret;

	ret = prctl(PR_GET_SECCOMP, 0, 0, 0, 0);
	if (ret < 0) {
		switch (errno) {
		case ENOSYS:
			printf("seccomp not available: pre-2.6.23\n");
			return 0;
		case EINVAL:
			printf("seccomp not available: not built in\n");
			return 0;
		default:
			fprintf(stderr, "unkonwn PR_GET_SECCOMP error: %s\n",
			strerror(errno));
			return 1;
		}
	}
	printf("seccomp available\n");

	ret = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, NULL, 0, 0);
	if (ret < 0) {
		switch (errno) {
		case EINVAL:
			printf("seccomp filter not available\n");
			return 0;
		case EFAULT:
			printf("seccomp filter available\n");
			return 0;
		default:
			fprintf(stderr, "unkonwn PR_SET_SECCOMP error: %s\n",
			strerror(errno));
			return 1;
		}
	}
	printf("PR_SET_SECCOMP unexpectedly succeeded?!\n");
	return 1;
}
