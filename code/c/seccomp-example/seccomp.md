## 介绍

`Seccomp`(secure computing)是Linux kernel （自从2.6.23版本之后）所支持的一种简洁的`sandboxing`机制。它能使一个进程进入到一种`安全`运行模式，该模式下的进程只能调用4种系统调用（system calls），即`read()`, `write()`, `exit()`和`sigreturn()`，否则进程便会被终止。

从Linux 3.5 开始，内核支持了特性`seccomp filter`（or `mode 2 seccomp`）。完整的`seccomp filter`特性的文档请参阅[这里][seccomp_filter]。

## 示例

[首先][seccomp-step-1],我们使用一个简单的程序来说明seccomp filter是否可以按照预期进行工作。该程序从标准输入读取用户的输入，并输出到标准输出中，在退出之前，它会调用函数`exit`。

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "config.h"


int main(int argc, char **argv) {
	char buf[1024];

	printf("Type stuff here: ");
	fflush(NULL);
	buf[0] = '\0';
	fgets(buf, sizeof(buf), stdin);
	printf("You typed: %s", buf);

	printf("And now we fork, which should do quite opposite...\n");
	fflush(NULL);
	sleep(1);

	fork();

	printf("You should not see this, because I am dead\n");

	return 0;
}
```

编译运行，输出如下

```bash
# autoconf
# autoheader 
# ./configure 
checking for gcc... gcc
checking whether the C compiler works... yes
checking for C compiler default output file name... a.out
checking for suffix of executables... 
checking whether we are cross compiling... no
checking for suffix of object files... o
checking whether we are using the GNU C compiler... yes
checking whether gcc accepts -g... yes
checking for gcc option to accept ISO C89... none needed
configure: creating ./config.status
config.status: creating config.h
# make
gcc -Wall   -c -o example.o example.c
gcc   example.o   -o example
# ./example
Type stuff here: hello seccomp
You typed: hello seccomp
And now we fork, which should do quite opposite...
You should not see this, because I am dead
You should not see this, because I am dead
```
该程序中所有的系统调用都能够正常工作。

## 添加 `seccomp filtering`

## 添加 `syscall reporting`

[seccomp-step-1]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-1
[seccomp-step-2]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-2
[seccomp-step-3]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-3
[seccomp-step-4]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-4
[seccomp_filter]: https://www.kernel.org/doc/Documentation/prctl/seccomp_filter.txt
