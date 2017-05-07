linux 系统调用`seccomp`用来管理调用进程的`Secure Computing state`。该API是在3.17的内核上添加的。

其原型如下：

```c
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <linux/signal.h>
#include <sys/ptrace.h>

int seccomp(unsigned int operation, unsigned int flags, void *args);
```

目前，在Linux系统上，`operation` 支持如下的值。

* `SECCOMP_SET_MODE_STRICT`

如果`operation`的值为`SECCOMP_SET_MODE_STRICT`时，`flags` 必须是0，`args`必须是NULL。此时，函数的执行效果等同于

```c
prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
```
该模式下，只有`read`,`write`,`_exit`和`sigreturn`四个系统调用可以使用，其他系统调用将会触发信号`SIGKILL`。

* `SECCOMP_SET_MODE_FILTER`

该模式下，可以通过参数`args`传递一个指向` Berkeley Packet Filter (BPF)`的指针。其数据结构为`struct sock_fprog`,它可以哟过来过滤任意的系统调用和系统调用参数。

使用`SECCOMP_SET_MODE_FILTER`时，调用要么需要具有权能`CAP_SYS_ADMIN`,要么进程需要设置`no_new_privs`位。如果`no_new_privs`位没有设置，我们可以通过如下的函数设置：

```c
prctl(PR_SET_NO_NEW_PRIVS, 1);
```

当`flags`为0时，函数的执行效果等同于

```c
prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, args);
```

关于seccomp更详细的细节介绍，请参阅[man seccomp][man7-seccomp]。

[man7-seccomp]: http://www.man7.org/linux/man-pages/man2/seccomp.2.html

下面的示例程序，可以根据用户的输入，禁用某个系统调用。
