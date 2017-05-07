#include <errno.h>
#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

static int install_filter(int nr, int arch, int error)
{
	struct sock_filter filter[] = {
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS,
			 (offsetof(struct seccomp_data, arch))),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, arch, 0, 3),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS,
			 (offsetof(struct seccomp_data, nr))),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, nr, 0, 1),
		BPF_STMT(BPF_RET+BPF_K,
			 SECCOMP_RET_ERRNO|(error & SECCOMP_RET_DATA)),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),
	};

	struct sock_fprog prog = {
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
		.filter = filter,
	};

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
		perror("prctl");
		return 1;
	}
	// Set seccomp mode to `SECCOMP_SET_MODE_FILTER`
	// 我们可以使用如下两种方式去设置，（1）prctl（2）seccomp

	/*if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) { */
	if (syscall(__NR_seccomp, SECCOMP_SET_MODE_FILTER, 0, &prog)) {
		perror("seccomp");
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 5) {
		fprintf(stderr, "Usage:\
%s <syscall_nr> <arch> <errno> <prog> [<args>]\n\
Hint for <arch>: AUDIT_ARCH_I386: 0x%X\n\
		 AUTIT_ARCH_X86_64: 0x%X\n\
", argv[0], AUDIT_ARCH_I386, AUDIT_ARCH_X86_64);
		exit(EXIT_FAILURE);
	}

	// 禁用命令行中指定的系统调用，并返回执行的错误码
	if (install_filter(strtol(argv[1], NULL, 0), strtol(argv[2], NULL, 0),
			   strtol(argv[3], NULL, 0)))
	{
		perror("install_filter");
		exit(EXIT_FAILURE);
	}

	execv(argv[4], &argv[4]);
	perror("execv");
	exit(EXIT_FAILURE);
}
