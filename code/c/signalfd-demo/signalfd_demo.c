#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define handle_error(msg)		\
	do {perror(msg); exit(EXIT_FAILURE);} while (0)

int main(int argc, char **argv) {
	sigset_t	mask;
	int sigfd;
	struct signalfd_siginfo	fdsi;
	ssize_t	s;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);

	// Block signals so that they are not handled
	// according to their default dispositions
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
		handle_error("sigprocmask");

	// 创建一个文件描述符，用来接收信号, `mask`说明了
	// 期望通过文件描述符接收的信号集合
	sigfd = signalfd(-1, &mask, 0);
	if (sigfd == -1)
		handle_error("signalfd");

	for(;;) {
		s = read(sigfd, &fdsi, sizeof(struct signalfd_siginfo));
		if (s != sizeof(struct signalfd_siginfo))
			handle_error("read");

		if (fdsi.ssi_signo == SIGINT) {
			printf("\nGot SIGINT\n");
		}else if (fdsi.ssi_signo == SIGQUIT) {
			printf("\nGot SIGQUIT\n");
			// 当收到信号`SIGQUIT`时，退出程序
			exit(EXIT_SUCCESS);
		}else {
			printf("\nRead unexpected signal\n");
		}
	}
}
