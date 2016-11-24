#  编译程序
$ gcc -c -o sockutil.o sockutil.c 
$ gcc -c -o userver.o userver.c 
$ gcc -c -o uclient.o uclient.c 
$ gcc -o userver userver.o sockutil.o
$ gcc -o uclient uclient.o sockutil.o

