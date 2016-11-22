该段代码用来判断当前内核上是否支持在容器里调用`reboot`系统调用[的特殊处理方式][reboot_pid_ns],

在linux 3.4 之后的内核版本中，当在非`initial PID namespace`中的其他`PID Namespace`调用`reboot`系统
调用，则会给其`PID Namespace`中的`init `进程发送信号。当命令为`LINUX_REBOOT_CMD_RESTART2`或者`LINUX_REBOOT_CMD_RESTART`
时，会发送信号`SIGHUP`，当命令为 `LINUX_REBOOT_CMD_POWER_OFF`或者`LINUX_REBOOT_CMD_HALT`时，会发送信号`SIGINT`。当为其他命令
时，`PID Namespace`中的`reboot`系统调用返回错误`-EINVAL`。

一般地，在容器引擎的实现中，如果内核不支持`reboot_pid_ns`，为了安全起见，则应该取消容器的权限`CAP_SYS_BOOT`，否则的话，
容器里的`reboot`调用会引起系统的重启。

[reboot_pid_ns]: https://github.com/torvalds/linux/commit/cf3f89214ef6a33fad60856bc5ffd7bb2fc4709b
