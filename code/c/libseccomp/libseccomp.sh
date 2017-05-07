$ gcc -o seccomp libseccomp.c `pkg-config --cflags --libs libseccomp`
$ ./seccomp
step 1: no seccomp filter added
step 2: only 'write' 'read','exit', 'rt_sigreturn' and dup2(1,2) syscalls are allowed
step 3: stderr redirected to stdout
Bad system call
$ echo $?
159
$ #  <------ 128+31 ==> SIGSYS
