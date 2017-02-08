$ gcc -o fifo_seqnum_server fifo_seqnum_server.c 
$ gcc -o fifo_seqnum_client fifo_seqnum_client.c 
$ ./fifo_seqnum_server &
$ ./fifo_seqnum_server &
[1] 6952
$ ./fifo_seqnum_client 3
0
$ ./fifo_seqnum_client 2
3
$ ./fifo_seqnum_client 
5
$ ./fifo_seqnum_client 6
6
$ ./fifo_seqnum_client 
12
