#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define PAT_SIZE	50
#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

int main(int argc, char **argv) {
	char pat[PAT_SIZE];
	char popenCmd[PCMD_BUF_SIZE];
	char pathname[PATH_MAX];
	int len, badPattern, fileCnt, status, j;
	FILE *fp;


	for (;;) {	// Read pattern, display results of globbing
		printf("pattern: ");
		fflush(stdout);
		if (fgets(pat, PAT_SIZE, stdin) == NULL)
			break;		// EOF
		len = strlen(pat);
		if (len < 1)	// Empty line
			continue;

		if (pat[len-1] == '\n')		// Strip trailing newline
			pat[len-1] = '\0';

		for (j = 0, badPattern = 0; j < len && !badPattern; j++) {
			if (!isalnum((unsigned char)pat[j]) &&
				strchr("_*?[^-].", pat[j]) == NULL)	// "_*?[^-]."这些是合法的字符
			badPattern = 1;
		}

		if (badPattern == 1) {
			printf("Bad pattern character: %c\n", pat[j-1]);
			continue;
		}

		// build and execute command to glob 'pat'
		snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);
		// ensure string is null-terminated
		popenCmd[PCMD_BUF_SIZE - 1] = '\0';

		fp = popen(popenCmd, "r");
		if (fp == NULL) {
			printf("popen() failed\n");
			continue;
		}

		// read resulting list of pathnames untils EOF
		fileCnt = 0;
		while (fgets(pathname, PATH_MAX, fp) != NULL) {
			printf("%s", pathname);
			fileCnt++;
		}

		// close pipe, fetch and display termination status
		status = pclose(fp);
		printf("	%d matching file%s\n", fileCnt, (fileCnt > 1) ? "s" : "");
		printf("	pclose() status == %#x\n", (unsigned int)status);
	}

	exit(EXIT_SUCCESS);
}
