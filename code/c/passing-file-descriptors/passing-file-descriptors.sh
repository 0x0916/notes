# 编译程序
$ gcc -o passing-file-descriptors passing-file-descriptors.c 

# 准备文件`passfd.txt`
$ echo "Hello, passing file descriptor" > passfd.txt
$ ./passing-file-descriptors passfd.txt 
Got file descriptor for 'passfd.txt'
The file descriptor is 4
Hello, passing file descriptor

