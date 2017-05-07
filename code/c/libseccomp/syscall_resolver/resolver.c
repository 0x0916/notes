#include <stdio.h>
#include <seccomp.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// Print the usage information and exit with EINVAL
static void exit_usage(const char *progname) {
	fprintf(stderr,
		"Usage: %s [-h] [-a <arch>] [-t] <name>|<number>\n",
		progname);
	exit(EINVAL);
}


int main(int argc, char **argv) {
	uint32_t arch;
	int opt;
	int translate = 0;
	int sys_num;
	const char *sys_name;

	arch = seccomp_arch_native();

	// parse the command line
	while ((opt = getopt(argc, argv, "a:ht")) > 0) {
		switch(opt) {
		case 'a':
			arch = seccomp_arch_resolve_name(optarg);
			if (arch == 0)
				exit_usage(argv[0]);
			break;
		case 't':
			translate = 1;
			break;
		case 'h':
		default:
			// usage infomation
			exit_usage(argv[0]);
			break;
		}
	}

	if (optind >= argc)
		exit_usage(argv[0]);

	// perform the syscall lookup
	if (isdigit(argv[optind][0]) || argv[optind][0] == '-') {
		sys_num = atoi(argv[optind]);
		sys_name = seccomp_syscall_resolve_num_arch(arch, sys_num);
		printf("%s\n", sys_name ? sys_name : "UNKNOWN");
	}else if (translate) {
		sys_num = seccomp_syscall_resolve_name_rewrite(arch, argv[optind]);
		printf("%d\n", sys_num);
	}else {
		sys_num = seccomp_syscall_resolve_name_arch(arch, argv[optind]);
		printf("%d\n", sys_num);
	}

	return 0;
}
