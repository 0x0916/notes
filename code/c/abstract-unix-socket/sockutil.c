#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sockutil.h"

// 打印错误，并结束程序的执行
void die(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

// 从文件描述符`from`拷贝数据到文件描述符`to`，直到没有数据为止。
// 如果出现错误，则退出
void copy_data(int from, int to) {
	char buf[1024];
	int amount;

	while ((amount = read(from, buf, sizeof(buf))) > 0) {
		if (write(to, buf, amount)!= amount) {
			die("write");
			return;
		}
	}

	if (amount < 0)
		die("read");

}

