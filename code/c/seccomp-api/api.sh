$ gcc -o dropper api.c
$ ./dropper
Usage:./dropper <syscall_nr> <arch> <errno> <prog> [<args>]
Hint for <arch>: AUDIT_ARCH_I386: 0x40000003
		 AUTIT_ARCH_X86_64: 0xC000003E
$  # 我们使用89这个错误代码
$ errno 89
EDESTADDRREQ 89 Destination address required
$  # 我们禁用掉函数execve
$ scmp_sys_resolver -a $(uname -m) -t execve
59
$ ./dropper 59 0xC000003E 89 /usr/bin/whoami
execv: Destination address required
$ # 禁用系统调用write
$ scmp_sys_resolver -a $(uname -m) -t write
1
$ ./dropper 1 0xC000003E 89 /usr/bin/whoami
$ # 禁用一个whoami不用的系统调用
$ scmp_sys_resolver -a $(uname -m) -t preadv
295
$ ./dropper 295 0xC000003E 89 /usr/bin/whoami
root

