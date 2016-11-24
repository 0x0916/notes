#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sockutil.h"

int main(int argc, char **argv) {
	struct sockaddr_un	address;
	int sock, conn;
	socklen_t addr_len;

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		die("socket");

	unlink("./sample-socket");

	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, "./sample-socket");

	addr_len = sizeof(address.sun_family) +
			sizeof(address.sun_path);

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
