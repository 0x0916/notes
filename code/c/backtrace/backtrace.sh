# The symbol names may be unavailable without the use of special linker
# options.  For systems using the GNU linker, it is necessary to use
# the -rdynamic linker option.  Note that names of "static" functions
# are not exposed, and won't be available in the backtrace.
$ gcc -rdynamic -o backtrace backtrace.c
$ ./backtrace 3
backtrace returned 8 addresses
./backtrace(func3+0x2e) [0x400ae4]
./backtrace() [0x400bb2]
./backtrace(func+0x25) [0x400bda]
./backtrace(func+0x1e) [0x400bd3]
./backtrace(func+0x1e) [0x400bd3]
./backtrace(main+0x59) [0x400c36]
