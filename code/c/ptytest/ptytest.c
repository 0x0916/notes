#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <pty.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>


// 当收到信号SIGWINCH时，改变该值
volatile  int sigwinch = 0;


// 信号SIGWINCH处理函数
void sigwinch_handler(int signal) {
	sigwinch = 1;
}

//char *ptsname(int);
int get_master_pty(char **name);
int get_slave_pty(char *name);

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
	char *name;
#define	BUFSIZE 1024
	char buf[BUFSIZE];

	if ((master = get_master_pty(&name)) < 0) {
		perror("could not open master pty");
		exit(1);
	}

	// 获得当前终端的大小
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0 ) {
		perror("could not get window size");
		exit(1);
	}

	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}

	// 子进程
	if (pid == 0) {
		int slave;

		// 子进程中不需要master端，关闭它
		close(master);

		if ((slave = get_slave_pty(name)) < 0) {
			perror("could not open slave pty");
			exit(1);
		}
		free(name);

		// 设置子进程会新的会话组leader
		if (setsid() < 0) {
			perror("could not set session leader");
		}

		// 设置控制终端
		if (ioctl(slave, TIOCSCTTY, NULL) ) {
			perror("could not set new controlling tty");
		}

		// 复制slave端到标准输入、标准输出和标准错误输出
		dup3(slave, STDIN_FILENO, 0);
		dup3(slave, STDOUT_FILENO, 0);
		dup3(slave, STDERR_FILENO, 0);

		if (slave > 2)
			close(slave);

		if (ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws) < 0) {
			perror("could not restore window size");
		}
		// 运行shell
		if ((ret = execl("/bin/bash", "/bin/bash", (char*)NULL)) < 0) {
			perror("execl");
			// 从不会运行到这里
			exit(1);
		}

	}

	// 父进程
	free(name);
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


// `get_master_pty`返回一个伪终端的master pty，并将对应的slave pty的名称通过`**name`返回
int get_master_pty(char **name) {
	int master = -1;
	char *slavename;

	// 打开伪终端克隆设备
	master = open("/dev/ptmx", O_RDWR);

	// 设置伪终端slave端的权限，并准许对其的访问
	if (master >= 0 && grantpt(master) >= 0 &&
		unlockpt(master) >= 0) {
		slavename = ptsname(master);

		if (!slavename) {
			close(master);
			master = -1;
		}else {
			*name = strdup(slavename);
			return master;
		}
	}

	return master;
}

// 返回伪终端slave端的文件描述符
int get_slave_pty(char *name) {
	int slave = -1;

	slave = open(name, O_RDWR);

	return slave;
}
