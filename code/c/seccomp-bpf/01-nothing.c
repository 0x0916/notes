#include <stdio.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>
#include <unistd.h>

int main(int argc, char **argv) {
	printf("step 1: unrestricted\n");

	// Enable filtering
	prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
	printf("step 2: only `read`, `write`, `_exit` and `sigreturn` syscalls allowed\n");

	// Redirect stderr to stdout
	dup2(1, 2);
	printf("step 3: !! YOU SHOULD NOT SEE ME!!\n");

	return 0;
}

/*
# gcc -o 01-nothing 01-nothing.c 
# ./01-nothing 
step 1: unrestricted
step 2: only `read`, `write`, `_exit` and `sigreturn` syscalls allowed
Killed
# echo "Status: $?"
Status: 137
# # 128 + 9 ==> SIGKILL
*/
