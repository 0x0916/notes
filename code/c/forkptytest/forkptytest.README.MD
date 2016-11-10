该程序利用了`-lutil`库中的函数`forkpty`来演示伪终端。

该程序非常简单，它创建了一个伪终端，在子进程中的slave pty中运行bash程序，这样在父进程的master pty就可以对bash进行操作。

该示例程序来自于：《Linux Application Development》 Page 396-400
