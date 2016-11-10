# 编译
$ gcc -o forkptytest forkptytest.c -lutil

# 查看当前的bash的进程号
$ echo $$
30690

# 运行程序，查看bash的进程号，与前面的不同。因为这个bash是在子进程中启动的。
$ ./forkptytest 
$  echo $$
30976
$ uname -a
Linux SZX1000042009 3.13.0-24-generic #47-Ubuntu SMP Fri May 2 23:30:00 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux

# 退出bash，并查看进程号，可以看出，我们启动的bash已经退出了。
$ exit
exit
$ echo $$
30690
