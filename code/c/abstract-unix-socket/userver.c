#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stddef.h>

#include "sockutil.h"

int main(int argc, char **argv) {
	struct sockaddr_un	address;
	int sock, conn;
	socklen_t addr_len;
	char *path = "./sample-socket";

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		die("socket");

	// 将address结构体清0
	memset(&address, 0, sizeof(address));

	address.sun_family = AF_UNIX;
	// `address.sun_path[0]` 已经被`memset`设置为0
	strncpy(&address.sun_path[1], path, strlen(path));

	// 注意长度恶计算
	addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(path) + 1;

	if (bind(sock, (struct sockaddr *) &address, addr_len))
		die("bind");

	if (listen(sock, 5))
		die("listen");

	while ((conn = accept(sock, (struct sockaddr *) &address, &addr_len)) >= 0) {
		printf("----------getting data\n");
		copy_data(conn, 1);
		printf("----------done\n");
		close(conn);
	}

	if (conn < 0)
		die("accept");

	close(sock);
	return 0;
}
