$  gcc -o signalfd_demo signalfd_demo.c 
$  ./signalfd_demo 
#  Control-C generates SIGINT
^C
Got SIGINT
^C
Got SIGINT
#  Control-\ generates SIGQUIT
^\
Got SIGQUIT 
$ # 程序退出
