`Unix domain socket`具有一个特别的能力——传递文件描述符。其他的IPC机制都不支持传递文件描述符。它允许进程打开一个文件，然后将文件描述符发送给另外一个进程（很可能不相关的进程）。

文件描述符使用如下的函数进行发送和接收：

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

为了演示传递文件描述符的方法，我们编写了一个示例程序（奇怪的cat），它实现了cat程序一样的功能，它接收一个文件名成作为其参数，在子进程中打开该文件，然后将文件描述符传递给父进程，父进程输入文件的内容到标准输出设备上。

关于`CMSG`的用法，请参考：http://www.man7.org/linux/man-pages/man3/cmsg.3.html

PS： 该示例参考自《Linux Application Development》second edition, Page 426-429


