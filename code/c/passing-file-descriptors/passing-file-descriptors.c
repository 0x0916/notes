#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

int child_process(char *filename, int sock);
int parent_process(int sock);
void copy_data(int form, int to);

int main(int argc, char **argv) {
	int socket[2];
	int status;

	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		return 1;
	}

	// 创建一对匿名的已经连接的socket
	// socket[0] 用于父进程
	// socket[1] 用于子进程
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, socket)) {
		perror("sockerpair error");
		exit(1);
	}


	// 创建子进程
	if (!fork()) {
		// Child Process
		close(socket[0]);
		return child_process(argv[1], socket[1]);
	}

	// Parent Process
	close(socket[1]);
	parent_process(socket[0]);

	// 回收子进程
	wait(&status);

	if (WEXITSTATUS(status))
		fprintf(stderr, "child failed\n");

	return 0;
}

// 子进程：发送文件描述符
int child_process(char *filename, int sock) {
	int fd;
	struct iovec	iov[1] = {0};
	struct msghdr	msg = {0};
	struct cmsghdr	*cmsg;
	int *fdptr;

	union {
		char buf[CMSG_SPACE(sizeof(int))];
		struct cmsghdr	align;
	} u;

	// 打开文件
	if ((fd = open(filename, O_RDONLY)) < 0) {
		perror("open");
		return 1;
	}

	// 在使用SOCK_STREAM传递文件描述符时，必须发送或者接收至少一个字节的nonancillary数据。
	iov[0].iov_base = filename;
	iov[0].iov_len = strlen(filename) + 1;

	// 通过SOCK_STREAM发送信息时，msg_name必须为NULL，msg_namelen必须为
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));

	// 初始化要发送的数据域,即将文件描述符拷贝到控制信息的末尾
	fdptr = (int *) CMSG_DATA(cmsg);
	memcpy(fdptr, &fd, sizeof(int));

	if (sendmsg(sock, &msg, 0) != iov[0].iov_len) {
		perror("sendmsg");
		exit(1);
	}

	return 0;
}

// 父进程：接收文件描述符
int parent_process(int sock) {
	struct msghdr	msg = {0};
	struct iovec	iov[1] = {0};
	struct cmsghdr	*cmsg;
	int *fdptr;
	int fd;
	char buf[80];

	union {
		char buf[CMSG_SPACE(sizeof(int))];
		struct cmsghdr align;
	} u;

	// 为了接收文件名做准备
	iov[0].iov_base = buf;
	iov[0].iov_len = 80;

	// 通过SOCK_STREAM接收信息时，msg_name必须为NULL，msg_namelen必须为
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);

	if (!recvmsg(sock, &msg, 0)) {
		perror("recvmsg");
		return 1;
	}

	cmsg = CMSG_FIRSTHDR(&msg);
	if (!cmsg) {
		perror("got NULL from CMSG_FIRSTHDR");
		return 1;
	}
	if (cmsg->cmsg_level != SOL_SOCKET) {
		fprintf(stderr, "expected SOL_SOCKET in cmsg: %d", cmsg->cmsg_level);
		return 1;
	}
	if (cmsg->cmsg_type != SCM_RIGHTS) {
		fprintf(stderr, "expected SCM_RIGHTS in cmsg: %d", cmsg->cmsg_type);
		return 1;
	}
	if (cmsg->cmsg_len != CMSG_LEN(sizeof(int))) {
		fprintf(stderr, "expected correct CMSG_LENin cmsg: %lu", cmsg->cmsg_len);
		return 1;
	}

	fdptr = (int *)CMSG_DATA(cmsg);
	if (!fdptr || *fdptr < 0) {
		fprintf(stderr, "recieved invalid pointer or file descriptor");
		return 1;
	}

	fd = *fdptr;
	printf("Got file descriptor for '%s'\n", (char *)iov[0].iov_base);
	printf("The file descriptor is %d\n", fd);

	copy_data(fd, 1);

	return 0;
}

// 将数据从文件描述符`from`中拷贝到`to`中，如果发送错误，则退出
void copy_data(int from, int to) {
	char buf[1024];
	int amount;

	while ((amount = read(from, buf, sizeof(buf))) > 0) {
		if (write(to, buf, amount) != amount) {
			perror("write");
			exit(1);
		}
	}

	if (amount < 0) {
		perror("read");
		exit(1);
	}
}
