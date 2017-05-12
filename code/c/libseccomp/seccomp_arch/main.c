#include <stdio.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


int main(int argc, char **argv)
{
	int rc = -1;
	scmp_filter_ctx ctx;

	// Initialize the seccomp filter state
	ctx = seccomp_init(SCMP_ACT_KILL);	// default action: KILL
	if (ctx == NULL)
		goto out;

	rc = seccomp_arch_exist(ctx, SCMP_ARCH_NATIVE);
	if ( rc == -EEXIST) {
		printf("SCMP_ARCH_NATIVE does not exist\n");
	}else if ( rc == 0) {
		printf("SCMP_ARCH_NATIVE exist\n");
	}

	rc = seccomp_arch_exist(ctx, SCMP_ARCH_X86_64);
	if (rc == -EEXIST) {
		printf("SCMP_ARCH_X86_64 does not exist\n");
		rc = seccomp_arch_add(ctx, SCMP_ARCH_X86_64);
		if (rc != 0)
			goto out;
		printf("SCMP_ARCH_X86_64 added\n");
		rc = seccomp_arch_remove(ctx, SCMP_ARCH_NATIVE);
		if (rc != 0)
			goto out;
		printf("SCMP_ARCH_NATIVE removed\n");
	} else if (rc == 0) {
		printf("SCMP_ARCH_X86_64 exist for default\n");
	}

	printf("SCMP_ARCH_NATIVE = %u\n", SCMP_ARCH_NATIVE);
	printf("SCMP_ARCH_X86_64= %u\n", SCMP_ARCH_X86_64);

out:
	seccomp_release(ctx);
	return -rc;
}
/*
SCMP_ARCH_NATIVE exist
SCMP_ARCH_X86_64 exist for default
SCMP_ARCH_NATIVE = 0
SCMP_ARCH_X86_64= 3221225534
*/
