$ gcc -o disc_sighup disc_sighup.c
# the `exec` command is a shell built-in command that causes the shell to do an exec()
# replacing itself with the named program. since the shell was the controlling process
# for the terminal, our program is now the controlling process and will receive
# `SIGHUP` when the terminal window is closed.
$ exec ./disc_sighup d s s > sig.log 2>&1
# Note: close the terminal window, and open another
$ cat sig.log 
PID of parent process is :	14555
Foreground process group ID is: 14555
PID=14965, PPID=14555		# first child in different process group
PID=14555, PPID=14451		# This is the parent process
PID=14966, PPID=14555		# remaining children are in the same process group as parent
PID=14967, PPID=14555
PID 14966: caught signal  1 (Hangup)
PID 14967: caught signal  1 (Hangup)

