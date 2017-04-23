/*
 * 示例1
 **/
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

int main(int argc, char **argv) {
	int ret, i;
	regex_t reg;
	regmatch_t pmatch[1];
	size_t	nmatch = 1;

	const char *pattern = "^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(.[a-zA-Z0-9_-]+)+$";
	char *text = "w-w@laoqinren.net";

	ret = regcomp(&reg, pattern, REG_EXTENDED);
	if (ret != 0) {
		printf("regcomp failed");
		exit(EXIT_FAILURE);
	}

	ret = regexec(&reg, text, nmatch, pmatch, 0);
	if (ret == REG_NOMATCH) {
		printf("NOMATCH\n");
	}else if (ret == 0){
		printf("MATCH\n");
		for (i = pmatch[0].rm_so; i < pmatch[0].rm_eo; ++i)
			putchar(text[i]);
		printf("\n");
	}

	regfree(&reg);

	return 0;
}
