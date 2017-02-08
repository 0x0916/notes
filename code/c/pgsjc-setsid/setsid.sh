$ ps -p $$ -o "pid pgid sid command"
   PID   PGID    SID COMMAND
$ gcc -o setsid setsid.c 
$ ./setsid 
PID=10358, PGID=10358, SID=10358
open /dev/tty: No such device or address
