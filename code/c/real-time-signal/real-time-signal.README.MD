`POSIX 信号模型`具有一些约束限制，比如：

* when a signal is pending sending that signal again does not result in multiple signal deliveries
* the lack of the ordering guarantees for the delivery of multiple different signals(is you send a SIGTERM followed by a SIGKILL there is no way of knowing which one will be delivered first)

`POSIX Real Time Signal` 通过新加了一些信号，解决了如上的限制，请请注意，对于移植性要求比较高的程序，建议使用`POSIX 信号模型`。

信号值在`SIGRTMIN`和`SIGRTMAX`之间的信号，都属于`Real-Time Signals`(POSIX并没有定义其确切的值).`Real-Time Signals`总是进行排队，每一个信号都可以发送到接收的进程而不会丢失。
信号值越小，该信号就会优先发送到接收的进程。同一个`Real-Time Signals`多次发送，进程会收到多次该信号，接收到的顺序跟信号发送的顺序一致。
