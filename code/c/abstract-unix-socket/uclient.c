#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stddef.h>

#include "sockutil.h"

int main(int argc, char **argv) {
	struct sockaddr_un	address;
	int sock;
	socklen_t addr_len;
	char *path = "./sample-socket";

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		die("socket");

	// 将address结构体清0
	memset(&address, 0, sizeof(address));

	address.sun_family = AF_UNIX;
	// `address.sun_path[0]` 已经被`memset`设置为0
	strncpy(&address.sun_path[1], path, strlen(path));

	// 注意长度的计算
	addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(path) + 1;

	if (connect(sock, (struct sockaddr *) &address, addr_len))
		die("connect");

	copy_data(0, sock);

	close(sock);
	return 0;
}
