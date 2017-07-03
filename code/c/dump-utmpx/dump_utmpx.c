#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <utmpx.h>
#include <string.h>

int main(int argc, char **argv) {
	struct utmpx *ut;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s [utmp-pathname]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc > 1) {
		if (utmpxname(argv[1]) == -1) {
			perror("utmpxname");
			exit(EXIT_FAILURE);
		}
	}

	setutxent();

	printf("user\t\ttype\t\tPID\tline\t\tid\t\thost\t\t\tdate/time\n");
	while ((ut = getutxent()) != NULL) {
		printf("%-8s", ut->ut_user);
		printf("\t%-9.9s",
			(ut->ut_type == EMPTY ) ? "EMPTY":
			(ut->ut_type == RUN_LVL) ? "RUN_LVL":
			(ut->ut_type == BOOT_TIME) ? "BOOT_TIME":
			(ut->ut_type == NEW_TIME) ? "NEW_TIME":
			(ut->ut_type == OLD_TIME) ? "OLD_TIME":
			(ut->ut_type == INIT_PROCESS) ? "INIT_PR":
			(ut->ut_type == LOGIN_PROCESS) ? "LOGIN_PR":
			(ut->ut_type == USER_PROCESS) ? "USER_PR":
			(ut->ut_type == DEAD_PROCESS) ? "DEAD_PR": "???");
		printf("\t%-5ld\t%-10.10s\t%-13.13s\t%-18.18s ", (long)ut->ut_pid,
			ut->ut_line, ut->ut_id, ut->ut_host);
		printf("\t%s", ctime((time_t*)&(ut->ut_tv.tv_sec)));

		memset(ut, 0, sizeof(struct utmpx));
	}
	endutxent();
	exit(EXIT_SUCCESS);
}
