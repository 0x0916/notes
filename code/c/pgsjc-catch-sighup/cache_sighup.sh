$ gcc -o cache_sighup cache_sighup.c 
# PID of shell is ID of session
$ echo $$
14617
# create two processes that remain in the process group created by the shell
$ ./cache_sighup > samegroup.log 2>&1 &
[1] 14682
# create a child that places itself in a separate process group
$ ./cache_sighup x  > diffgroup.log 2>&1 &
[2] 14684

# NOTE: close the terminal windows and open another
$ cat samegroup.log 
PID=14682, PPID=14617, PGID=14682, SID=14617	# Parent
PID=14683, PPID=14682, PGID=14682, SID=14617	# Child
14682: caught signal SIGHUP
14683: caught signal SIGHUP
$ cat diffgroup.log 
PID=14684, PPID=14617, PGID=14684, SID=14617	# Parent
PID=14685, PPID=14684, PGID=14685, SID=14617	# Child
14684: caught signal SIGHUP			# Parent was signaled, but not child
