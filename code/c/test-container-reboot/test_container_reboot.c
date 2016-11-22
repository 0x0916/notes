#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

static int container_reboot_supported(void *arg)
{
	int *cmd = arg;
	int ret;

	ret = reboot(*cmd);
	if (ret == -1 && errno == EINVAL) {
		// 支持，返回1
		return 1;
	}

	// 不支持，返回0
	return 0;
}

int main(int argc, char **argv) {
	FILE *f;
	long stack_size = 4096;
	void *stack = alloca(stack_size);
	pid_t	pid;
	int ret, cmd, v, status;

	// 可以通过命令LINUX_REBOOT_CMD_CAD_ON、LINUX_REBOOT_CMD_CAD_OFF来检查内核是否支持
	// 在容器里处理`reboot`系统调用。
	f = fopen("/proc/sys/kernel/ctrl-alt-del", "r");
	if (!f) {
		fprintf(stderr, "Failed to open /proc/sys/kernel/ctrl-alt-del\n");
		exit(1);
	}

	ret = fscanf(f, "%d", &v);
	fclose(f);
	if (ret != 1) {
		fprintf(stderr, "Failed to read /proc/sys/kernel/ctrl-alt-del\n");
		exit(1);
	}

	// 为了不修改当前系统的`ctrl-alt-del`配置，我们根据该配置的值，给`reboot`传递相应
	// 的命令。
	cmd = v ? LINUX_REBOOT_CMD_CAD_ON: LINUX_REBOOT_CMD_CAD_OFF;

	// 创建`子PID Namespace`进程
	pid = clone(container_reboot_supported, stack+stack_size, CLONE_NEWPID | SIGCHLD, &cmd);
	if (pid < 0) {
		fprintf(stderr,"failed to clone : %m\n");
		exit(1);
	}

	if (wait(&status) < 0) {
		fprintf(stderr, "unexpected wait error: %m\n");
		exit(1);
	}

	if (WEXITSTATUS(status) == 1)
		printf("Support Container Reboot\n");
	else
		printf("Not Support Container Reboot\n");
}

