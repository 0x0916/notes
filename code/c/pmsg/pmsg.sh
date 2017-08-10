root@localhost:pmsg# ./pmsg_create  -xc /mq
root@localhost:pmsg# ./pmsg_getattr  /mq
Maximum # of messages on queue:         10
Maximum message size:                   8192
# of messages currently on queue        0
root@localhost:pmsg# ls -l /dev/mqueue/
total 0
-rw------- 1 root root 80 Aug 10 16:19 mq
root@localhost:pmsg# cat  /dev/mqueue/mq 
QSIZE:0          NOTIFY:0     SIGNO:0     NOTIFY_PID:0     
root@localhost:pmsg# ./pmsg_unlink  /mq
root@localhost:pmsg# cat  /dev/mqueue/mq 
cat: /dev/mqueue/mq: No such file or directory
root@localhost:pmsg#
