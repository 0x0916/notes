## 检测系统是否支持`seccomp`特性

一般来说，内核对`seccomp`的支持有以下三种情况：

* 不支持seccomp
* 仅支持原始的模式（mode 1）
* 支持secommp filtering（bpf）

如果内核不支持seccomp时，函数`prctl(PR_GET_SECCOMP,...);`会返回ENOSYS(2.6.23　之前的内核) 或者EINVAL(没有编译进内核)。


在2.6.23和3.4之间的内核中，某些架构下可能未实现`filtering`特性，我们可以通过函数`prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, NULL, 0, 0);`进行检测，如果返回`EINVAL`，说明仅支持原始模式，返回`EFAULT`说明支持`seccomp filtering`。


完整的检测代码如下：



