#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	// `isatty` 判断指定的文件描述符引用的是否是一个终端
	printf("fd 0: %s\n", isatty(0) ? "tty" : "not a tty");
	printf("fd 1: %s\n", isatty(1) ? "tty" : "not a tty");
	printf("fd 2: %s\n", isatty(2) ? "tty" : "not a tty");
}
