#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sockutil.h"

int main(int argc, char **argv) {
	struct sockaddr_un	address;
	int sock;
	socklen_t addr_len;

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		die("socket");

	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, "./sample-socket");

	addr_len = sizeof(address.sun_family) +
			sizeof(address.sun_path);

	if (connect(sock, (struct sockaddr *) &address, addr_len))
		die("connect");

	copy_data(0, sock);

	close(sock);
	return 0;
}
