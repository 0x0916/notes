### pty

伪终端（pty）是提供双向通信管道的一对虚拟的字符设备。该管道的一端称为**master**，另外一端称为**slave**。

伪终端的slave端更像经典的终端，一般情况下，一个进程可以通过打开伪终端的slave端，将其作为自己的终端，这样的话，拥有伪终端的master端的进程就可以控制该进程。

任何写到伪终端master上东西都会作为salve端进程的输入。例如：向master字符设备写入中断字符（`Ctrl+C`），连接到slave端的进程就会收到中断信号（SIGINT）。 相反的，任何写到伪终端slave端字符设备的东西都可以被master端的进程读取到。

在master端和slave端的数据量时异步的。

由于历史原因，有两套伪终端的API，`BSD-style`和 `System-V style`。`System-v style` 在linux系统上又称为 UNIX 98 伪终端。在新的应用程序中，我们应该使用Unix 98风格的伪终端。



#### Unix 98 pseudoterminals

`posix_openpt`用来打开下一个可用的伪终端master设备。在伪终端slave设备可使用前，必须设置它的权限，使得应用程序可以访问它。`grantpt`可以将slave设备的用户ID设置为实际调用者的ID。

`unlockpt`用于准予对伪终端slave设备的访问，从而允许应用程序打开该设备。

在`grantpt`和`unlockpt`这两个函数中，文件描述符是与伪终端master设备关联的文件描述符。

`ptsname`函数用于在给定伪终端master设备的文件描述符时，找到伪终端slave设备的路径名。这使应用程序可以独立于给定平台的某种惯例而标识伪终端slave设备。

在linux内核中，系统允许的最大伪终端数目有一个限制，我们可以通过接口`/proc/sys/kernel/pty/max`动态的调整它，另外，接口`/proc/sys/kernel/pty/nr`显示了目前系统上正在使用的伪终端数目。



#### BSD pseudoterminals

基于BSD的伪终端中，master设备和slave设备必须提前创建好。主设备为`/dev/ptyXY`,从设备为`/dev/ttyXX`。打开伪终端时，我们必须自己确定第一个可用的PTY主设备，为了达到该目的，从`/dev/ptyp0`开始不断进行尝试，知道成功打开一个可用的PTY主设备。一旦成功打开一个名称为`/dev/ptyMN`的主设备，那么对应的从设备名称为`/dev/ttyMN`。
