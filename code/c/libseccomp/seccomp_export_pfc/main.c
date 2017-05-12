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
	int filter_fd;

	// Initialize the seccomp filter state
	ctx = seccomp_init(SCMP_ACT_KILL);	// default action: KILL
	if (ctx == NULL)
		goto out;

	// setup basic whitelist
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigreturn), 0);

	// setup our rule
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup2), 2,
			SCMP_A0(SCMP_CMP_EQ, 1),
			SCMP_A1(SCMP_CMP_EQ, 2));

	filter_fd = open("./out.pfc", O_CREAT|O_WRONLY);
	if (filter_fd == -1) {
		rc = -errno;
		perror("open");
		goto out;
	}

	// Export the seccomp filter
	rc = seccomp_export_pfc(ctx, filter_fd);
	if (rc < 0) {
		close(filter_fd);
		perror("seccomp_export_bpf");
		goto out;
	}

	close(filter_fd);
out:
	seccomp_release(ctx);
	return -rc;
}
