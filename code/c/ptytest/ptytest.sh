# 编译
$ gcc -o ptytest ptytest.c 

#  查看`bash`的进程号
$ echo $$
30690

# 运行程序，查看`bash`的进程号，此时`bash`为子进程中运行的
$ ./ptytest 
$ uname -r
3.13.0-24-generic
$ echo $$
31890

# 退出`bash`
$ exit
exit
$ echo $$
30690

