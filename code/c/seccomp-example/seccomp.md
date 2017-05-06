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
[接下来][seccomp-step-2]，我们为该程序添加了基本的`seccomp filter`，即允许基本的系统调用（`read`,`write`,`signal handling`,`exit`等等）。如果遇到其他系统调用，则会`kill`掉该进程。

```c
diff --git a/configure.ac b/configure.ac
index f68a004..048fca6 100644
--- a/configure.ac
+++ b/configure.ac
@@ -2,4 +2,5 @@ AC_INIT([example], [0.1], [w@laoqinren.net], [example], [http://example.com/])
 AC_PREREQ([2.59])
 AC_CONFIG_HEADERS([config.h])
 AC_PROG_CC
+AC_CHECK_HEADERS([linux/seccomp.h])
 AC_OUTPUT
diff --git a/example.c b/example.c
index e00e472..2e7b6a6 100644
--- a/example.c
+++ b/example.c
@@ -5,11 +5,54 @@
 #include <stdlib.h>
 
 #include "config.h"
+#include "seccomp-bpf.h"
 
+static int install_syscall_filter(void)
+{
+	struct sock_filter filter[] = {
+		/* Validate architecture. */
+		VALIDATE_ARCHITECTURE,
+		/* Grab the system call number. */
+		EXAMINE_SYSCALL,
+		/* List allowed syscalls. */
+		ALLOW_SYSCALL(rt_sigreturn),
+#ifdef __NR_sigreturn
+		ALLOW_SYSCALL(sigreturn),
+#endif
+		ALLOW_SYSCALL(exit_group),
+		ALLOW_SYSCALL(exit),
+		ALLOW_SYSCALL(read),
+		ALLOW_SYSCALL(write),
+		KILL_PROCESS,
+	};
+	struct sock_fprog prog = {
+		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
+		.filter = filter,
+	};
+
+	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
+		perror("prctl(NO_NEW_PRIVS)");
+		goto failed;
+	}
+	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) {
+		perror("prctl(SECCOMP)");
+		goto failed;
+	}
+	return 0;
+
+failed:
+	if (errno == EINVAL)
+		fprintf(stderr, "SECCOMP_FILTER is not available. :(\n");
+	return 1;
+}
 
 int main(int argc, char **argv) {
 	char buf[1024];
 
+
+	if (install_syscall_filter())
+		return 1;
+
 	printf("Type stuff here: ");
 	fflush(NULL);
 	buf[0] = '\0';
```

此时，我们也不知道运行该程序时，还需要什么系统调用，其运行效果如下：

```bash
# make
gcc -Wall   -c -o example.o example.c
gcc   example.o   -o example
# ./example
Bad system call
# echo $?
159
```

## 添加 `syscall reporting`

[现在][seccomp-step-3]，我们可以利用`seccomp-filter`的特性，在其捕捉到系统调用失败时，显示出详细的信息而不是立刻退出。当然，我们在找到所有该程序依赖的系统调用后，就可以删除该部分代码逻辑了。

```c
diff --git a/Makefile b/Makefile
index 96d074c..2625ee5 100644
--- a/Makefile
+++ b/Makefile
@@ -3,7 +3,9 @@ CFLAGS=-Wall
 
 all: example
 
-example: example.o
+include syscall-reporter.mk
+
+example: example.o syscall-reporter.o
 
 .PHONY: clean
 clean:
diff --git a/example.c b/example.c
index 7ad4fc0..7a0f276 100644
--- a/example.c
+++ b/example.c
@@ -6,6 +6,7 @@
 
 #include "config.h"
 #include "seccomp-bpf.h"
+#include "syscall-reporter.h"
 
 static int install_syscall_filter(void)
 {
@@ -23,6 +24,7 @@ static int install_syscall_filter(void)
 		ALLOW_SYSCALL(exit),
 		ALLOW_SYSCALL(read),
 		ALLOW_SYSCALL(write),
+		/* Add more syscalls here */
 		KILL_PROCESS,
 	};
 	struct sock_fprog prog = {
@@ -50,6 +52,8 @@ int main(int argc, char **argv) {
 	char buf[1024];
 
 
+	if (install_syscall_reporter())
+		return 1;
 	if (install_syscall_filter())
 		return 1;
 
```

编译运行如下，发现其确实一些系统调用，比如`fstat`,`nanosleep`,`brk`和`clone`等。

```bash
# make
gcc -Wall   -c -o example.o example.c
In file included from example.c:9:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
echo "static const char *syscall_names[] = {" > syscall-names.h ;\
echo "#include <syscall.h>" | cpp -dM | grep '^#define __NR_' | \
	LC_ALL=C sed -r -n -e 's/^\#define[ \t]+__NR_([a-z0-9_]+)[ \t]+([0-9]+)(.*)/ [\2] = "\1",/p' >> syscall-names.h ;\
echo "};" >> syscall-names.h
gcc -Wall   -c -o syscall-reporter.o syscall-reporter.c
In file included from syscall-reporter.c:12:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
gcc   example.o syscall-reporter.o   -o example
# ./example
Looks like you also need syscall: fstat(5)
# vim example.c
# make
gcc -Wall   -c -o example.o example.c
In file included from example.c:9:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
gcc   example.o syscall-reporter.o   -o example
# ./example
Looks like you also need syscall: brk(12)
# vim example.c
# make
gcc -Wall   -c -o example.o example.c
In file included from example.c:9:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
gcc   example.o syscall-reporter.o   -o example
# ./example
Type stuff here: hello seccomp
You typed: hello seccomp
And now we fork, which should do quite opposite...
Looks like you also need syscall: nanosleep(35)
# vim example.c
# make
gcc -Wall   -c -o example.o example.c
In file included from example.c:9:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
gcc   example.o syscall-reporter.o   -o example
# ./example
Type stuff here: hello seccomp
You typed: hello seccomp
And now we fork, which should do quite opposite...
Looks like you also need syscall: clone(56)
# vim example.c
# make
gcc -Wall   -c -o example.o example.c
In file included from example.c:9:0:
syscall-reporter.h:21:2: warning: #warning "You've included the syscall reporter. Do not use in production!" [-Wcpp]
 #warning "You've included the syscall reporter. Do not use in production!"
  ^
gcc   example.o syscall-reporter.o   -o example
# ./example
Type stuff here: hello seccomp
You typed: hello seccomp
And now we fork, which should do quite opposite...
You should not see this, because I am dead
You should not see this, because I am dead
# ...
```
###  测试完成

[这里][seccomp-step-4]是最后的代码，我们需要添加的系统调用如下：

```c
diff --git a/example.c b/example.c
index ce53a64..9e23e81 100644
--- a/example.c
+++ b/example.c
@@ -24,6 +24,11 @@ static int install_syscall_filter(void)
                ALLOW_SYSCALL(read),
                ALLOW_SYSCALL(write),
                /* Add more syscalls here */
+               ALLOW_SYSCALL(fstat),
+               ALLOW_SYSCALL(brk),
+               ALLOW_SYSCALL(nanosleep),
+               ALLOW_SYSCALL(clone),
                KILL_PROCESS,
        };
        struct sock_fprog prog = {

```
[seccomp-step-1]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-1
[seccomp-step-2]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-2
[seccomp-step-3]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-3
[seccomp-step-4]: https://github.com/0x0916/notes/tree/master/code/c/seccomp-example/seccomp-step-4
[seccomp_filter]: https://www.kernel.org/doc/Documentation/prctl/seccomp_filter.txt
