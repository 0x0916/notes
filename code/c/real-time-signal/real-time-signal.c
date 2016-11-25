#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <string.h>
#include <unistd.h>


int next_sig = 0;
int sig_order [10];

// 信号处理函数，用来记录信号的处理顺序
void handler(int signo) {
	sig_order[next_sig++] = signo;
}

int main(int argc, char **argv) {
	sigset_t	mask;
	sigset_t	old_mask;
	struct	sigaction	act;
	int i;

	// 该程序中要处理的信号集合
	sigemptyset(&mask);
	sigaddset(&mask, SIGRTMIN);
	sigaddset(&mask, SIGRTMIN+1);
	sigaddset(&mask, SIGUSR1);

	act.sa_handler = handler;
	// 设置`sa_mask`的目的是：防止信号处理函数handler中的竞争。
	act.sa_mask = mask;
	act.sa_flags = 0;

	sigaction(SIGRTMIN, &act, NULL);
	sigaction(SIGRTMIN+1, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);

	// BLOCK这些信号，以方便我们查看这些信号的处理顺序
	sigprocmask(SIG_BLOCK, &mask, &old_mask);

	// 使用`raise`给当前进程发送信号
	raise(SIGRTMIN + 1);
	raise(SIGRTMIN);
	raise(SIGRTMIN);
	raise(SIGRTMIN + 1);
	raise(SIGRTMIN);
	raise(SIGUSR1);
	raise(SIGUSR1);

	// 使能信号的传递，信号处理完后，该函数才返回。
	sigprocmask(SIG_SETMASK, &old_mask, NULL);

	printf("signals received:\n");
	for (i = 0; i < next_sig; i++) {
		if (sig_order[i] < SIGRTMIN)
			printf("\t%s\n", strsignal(sig_order[i]));
		else
			printf("\tSIGRTMIN + %d\n", sig_order[i] - SIGRTMIN);
	}

	return 0;
}
