系统调用`execve`可以给与新进程增加权限，特别是新进程的父进程所没有的权限。
最明显的例子就是运行的程序是设置了`setuid/setgid`或者`file capabilities`的程序。
为了防止父进程或得这些特权，内核和用户代码就必须进行小心的处理，有很多中临时的办法解决这个问题。


从Linux 3.5之后的内核中，添加了一个新的、通用的机制`no_new_privs`可以让进程安全的修改其执行环境。
所有的进程都可以可以设置`no_new_privs`，一旦设置了，`fork`，`clone`，`execve`都会继承该设置，且
不能被取消设置。

设置了`no_new_privs`后，`execve`保证了进程不会获取未执行execve时没有的任何特权。例如：

* `setuid`和`setgid`位将不会改变`uid`和`gid`。
* `file capabilites`将不会被添加到`permitted`中。

可以使用如下方式设置`no_new_privs`:

```c
prctl(PR_SET_NO_NEW_PRIVS 1 0,0,0)。
```

目前主要有以下两种场景使用了`no_new_privs`:

* seccomp filter mode 2
* 程序自己使用，来减少非特权用户的攻击面。
