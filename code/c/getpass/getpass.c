#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>

#define MAX_PASS_LEN	16
char *getpass(const char *);

int main(int argc, char **argv) {
	char *ptr;

	if ((ptr = getpass("Enter password:")) == NULL) {
		printf("getpass error\n");
		exit(1);
	}
	printf("password: %s\n", ptr);

	/* now use password (probably encrypt it) ... */

	while (*ptr != 0)
		*ptr++ = 0;

	exit(0);
}

char *getpass(const char *prompt) {
	static char	buf[MAX_PASS_LEN-1];
	sigset_t	sig, osig;
	struct termios	ts, ots;
	int		c;
	char		*ptr;
	FILE *fp;

	// 调用`ctermid`函数打开控制终端，而不是直接将`/dev/tty`写在程序中
	if ((fp = fopen(ctermid(NULL),"r+")) == NULL)
		return NULL;

	setbuf(fp, NULL);

	sigemptyset(&sig);
	// Block SIGINT SIGTSTP
	sigaddset(&sig, SIGINT);
	sigaddset(&sig, SIGTSTP);
	// and save mask
	sigprocmask(SIG_BLOCK, &sig, &osig);

	// save tty state
	tcgetattr(fileno(fp), &ts);
	ots = ts;
	ts.c_lflag &= ~(ECHO | ECHOE| ECHOK | ECHONL);
	tcsetattr(fileno(fp), TCSAFLUSH, &ts);
	fputs(prompt, fp);

	ptr = buf;
	while ((c = getc(fp)) != EOF && c != '\n')
		if (ptr < &buf[MAX_PASS_LEN])
			*ptr++ = c;
	*ptr = 0;	// null terminate
	putc('\n', fp);	// echo a newline

	// restore TTY state
	tcsetattr(fileno(fp), TCSAFLUSH, &ots);
	// restore mask
	sigprocmask(SIG_SETMASK, &osig, NULL);
	fclose(fp);

	return buf;
}
