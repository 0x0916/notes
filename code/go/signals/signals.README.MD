有时，我们会希望Go程序处理一些[Unix信号](https://en.wikipedia.org/wiki/Unix_signal)，例如，当一个服务器程序收到信号`SIGTERM`后，做一些清理工作，然后再关闭。通过下面的例子，我们来学习在Go中如何处理信号。

一般情况下，信号`SIGKILL` 和 `SIGSTOP` 不能被程序捕获，所以Go的信号处理相关函数也不会影响这两个信号。
