#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <pty.h>
#include <errno.h>

#include <sys/poll.h>

// 当收到信号SIGWINCH时，改变该值
volatile  int sigwinch = 0;


// 信号SIGWINCH处理函数
void sigwinch_handler(int signal) {
	sigwinch = 1;
}

int main(int argc, char **argv) {
	int pid;
	int master;
	struct winsize ws;
	struct sigaction act;
	int done = 0;
	struct termios ot, t;
	struct pollfd ufds[2];
	int i;
	int ret;
#define	BUFSIZE 1024
	char buf[BUFSIZE];

	// 获得当前终端的大小
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0 ) {
		perror("could not get window size");
		exit(1);
	}

	// create a new process operating in a pseudoterminal
	if ((pid = forkpty(&master, NULL,NULL, &ws)) < 0) {
		perror("forkpty");
		exit(1);
	}

	// 子进程
	if (pid == 0) {
		// 运行shell
		if ((ret = execl("/bin/bash", "/bin/bash", (char*)NULL)) < 0) {
			perror("execl");
			// 从不会运行到这里
			exit(1);
		}

	}

	// 父进程

	//注册信号处理函数
	act.sa_handler = sigwinch_handler;
	sigemptyset(&(act.sa_mask));
	act.sa_flags = 0;
	if (sigaction(SIGWINCH, &act, NULL) < 0) {
		perror("could not handl SIGWINCH");
		exit(1);
	}

	tcgetattr(STDIN_FILENO, &ot);
	t = ot;
	t.c_lflag &= ~(ICANON | ISIG | ECHO| ECHOCTL| ECHOE \
			| ECHOK| ECHOKE| ECHONL | ECHOPRT);
	t.c_iflag |= IGNBRK;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);

	// 监听父进程的标准输入和伪终端的master端
	ufds[0].fd = STDIN_FILENO;
	ufds[0].events = POLLIN;
	ufds[1].fd = master;
	ufds[1].events = POLLIN;

	do {
		int r;

		// 监听文件描述符
		r = poll(ufds, 2, -1);
		if ((r<0) && (errno != EINTR))	 {
			done = 1;
			break;
		}

		// 检测是否达到退出的条件
		if ((ufds[0].revents | ufds[1].revents) &
				(POLLERR| POLLHUP| POLLNVAL)) {
			done = 1;
			break;
		}

		// 如果需要，改变终端窗口的大小
		if (sigwinch ) {
			if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0 ) {
				perror("could not get window size");
			}

			if (ioctl(master, TIOCSWINSZ, &ws) < 0 ) {
				perror("could not restore window size");
			}

			sigwinch = 0;
			continue;
		}

		// 当伪终端的master端有数据时，读取他，并将它写入父进程的标准输出中
		if (ufds[1].revents & POLLIN) {
			i = read(master, buf, BUFSIZE);
			if (i >= 1)
				write(STDOUT_FILENO, buf, i);
			else
				done = 1;
		}

		// 当标准输入有数据时，读取它，并将它写入伪终端的master端
		if (ufds[0].revents & POLLIN) {
			i = read(STDIN_FILENO, buf, BUFSIZE);
			if (i >= 1)
				write(master, buf, i);
			else
				done = 1;
		}
	}while (!done);

	tcsetattr(STDIN_FILENO, TCSANOW, &ot);
	exit(0);
}
