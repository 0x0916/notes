/*
 * 示例2
 **/
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

char *substr(const char *str, unsigned start, unsigned end) {
	unsigned n = end - start;
	char *retstr = malloc((n+1)*sizeof(char));

	strncpy(retstr, str+start, n);
	retstr[n] = 0;

	return retstr;
}

int main(int argc, char **argv) {
	int ret, lno = 0, i;
	regex_t	reg;
	char ebuf[128], lbuf[256];
	regmatch_t	*pmatch = NULL;
	size_t	nmatch;
	char *pattern = NULL;

	if (argc < 2) {
		printf("Usage: %s <regex expression>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pattern = argv[1];

	// 编译正则表达式
	ret = regcomp(&reg, pattern, REG_EXTENDED);
	if (ret != 0) {
		regerror(ret, &reg, ebuf, sizeof(ebuf));
		printf("regcomp failed: %s\n", ebuf);
		exit(EXIT_FAILURE);
	}

	// 申请存放匹配结果的内存
	nmatch = reg.re_nsub;
	pmatch = (regmatch_t *)malloc((nmatch+1) * sizeof(regmatch_t));
	if (!pmatch) {
		printf("malloc failed.\n");
		exit(EXIT_FAILURE);
	}

	while (fgets(lbuf, sizeof(lbuf), stdin)) {
		lno++;
		if (lbuf[strlen(lbuf)-1] == '\n')
			lbuf[strlen(lbuf)-1] = 0;

		ret = regexec(&reg, lbuf, nmatch+1, pmatch, 0);
		if (ret == REG_NOMATCH) {
			continue;
		} else if (ret != 0) {
			regerror(ret, &reg, ebuf, sizeof(ebuf));
			printf("regexec failed: %s\n", ebuf);
			continue;
		}
		// 输出匹配结果
		for (i = 0; i < nmatch+1 && pmatch[i].rm_so != -1; i++) {
			char *temp;
			if (i == 0)
				printf("%04d: %s\n",lno, lbuf);
			temp = substr(lbuf, pmatch[i].rm_so, pmatch[i].rm_eo);
			printf("\t$%d='%s'\n", i, temp);
			free(temp);
		}
	}

	regfree(&reg);
	return 0;
}
