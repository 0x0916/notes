$ gcc -o new-intr new-intr.c   # 编译
# setting interrupt character to Control-L(ASCII 12)
$ ./new-intr 12
# verfiy the change with stty
$ stty 
speed 38400 baud; line = 0;
intr = ^L;
-brkint -imaxbel
# start a process running sleep(1)
$ sleep 10
^C^L		# Control-C has no effect; it is just echoed,
# Type Control-L to terminate sleep
$ echo $?
130		# singal number is 2 (130-128)
# disable the integrpt character
$ ./new-intr 
# verfiy the change with stty
$ stty 
speed 38400 baud; line = 0;
intr = <undef>;
-brkint -imaxbel
# start a process running sleep(1)
$ sleep 10
^C^L	# neither Control-C nor Control-L generates a SIGINT signal
Quit
# Type Control-\ to terminate sleep
$ stty 
speed 38400 baud; line = 0;
intr = <undef>;
-brkint -imaxbel
# return terminal to a sane state
$ stty sane
