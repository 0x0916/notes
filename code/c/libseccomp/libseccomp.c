#include <stdio.h>
#include <unistd.h>
#include <seccomp.h>

int main(int argc, char **argv)
{
	int rc = -1;
	scmp_filter_ctx ctx;

	printf("step 1: no seccomp filter added\n");

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

	// load the filter
	rc = seccomp_load(ctx);
	if (rc < 0)
		goto out;

	printf("step 2: only 'write' 'read','exit', 'rt_sigreturn' and dup2(1,2) syscalls are allowed\n");

	// redirect stderr to stdout
	dup2(1, 2);
	printf("step 3: stderr redirected to stdout\n");

	// Duplicate stderr to arbitrary fd
	dup2(2, 23);
	printf("step 4: !! YOU SHOULD NOT SEE ME !!\n");
out:
	// Release the seccomp filter state
	seccomp_release(ctx);

	return -rc;
}
