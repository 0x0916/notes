$ gcc pipe-sync.c  -o pipe-sync
$ ./pipe-sync 
./pipe-sync sleep-time...
$ ./pipe-sync 3 4 5 6 7
17:29:17 Parent started
17:29:20 child 1 (PID=2488) closing pipe
17:29:21 child 2 (PID=2489) closing pipe
17:29:22 child 3 (PID=2490) closing pipe
17:29:23 child 4 (PID=2491) closing pipe
17:29:24 child 5 (PID=2492) closing pipe
17:29:24 Parent ready to go
