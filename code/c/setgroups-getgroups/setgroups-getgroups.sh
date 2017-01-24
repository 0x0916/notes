$ gcc -o setgroups-getgroups setgroups-getgroups.c 
$ id -u   # 以root用户运行程序
0
$ id -g
0
$ ./setgroups-getgroups 
the total number of groups: 1
(0) group ID: 0
after getgroups
(0) group ID: 0
(1) group ID: 1000
$ id -u		# 以非root用户运行程序
1000
$ id -g
1000
$ ./setgroups-getgroups 
the total number of groups: 9
(0) group ID: 4
(1) group ID: 24
(2) group ID: 27
(3) group ID: 30
(4) group ID: 46
(5) group ID: 113
(6) group ID: 128
(7) group ID: 999
(8) group ID: 1000
setgroups error: Operation not permitted
after getgroups
(0) group ID: 4
(1) group ID: 24
(2) group ID: 27
(3) group ID: 30
(4) group ID: 46
(5) group ID: 113
(6) group ID: 128
(7) group ID: 999
(8) group ID: 1000
