## Process and Programs

* A *process* is an instance of an executing program.
* A *program* is a file containing a range of information that describes how to construct a process at run time.
 - Binary format identification
 - Machine-language instructions
 - Program entry-point address
 - Data
 - Symbol and relocation tables
 - Share-library and dynamic-linking information
 - Other information

Also, we give the following definition for a process: a process is an abstract entity, defined by the kernel, to which system resources are allocated in order to execute a program.

## Process ID and Parent Process ID

Each process has a process ID(`PID`). a positive integer that uniquely identifies the process on the system.

The Linux Kernel limits process IDs to being less than or equal to `32767`. Each time the limit of 32767 is reached, the kernel resets its process ID counter so that process IDs are assigned starting from low integer values. (Once it has reached 32767, the process ID counter is reset to 300, rather that 1).

The limit is adjustable via the value in the linux-specific `/proc/sys/kernel/pid_max`

Each process has a parent--the process that create it. If a child process becomes orphaned because its "birth" parent terminates. then the child is adopted by the *init* process.

The parent of any process can be found by looking at the `Ppid` filed provided in the Linux-specific `/proc/PID/status` file.

## Memory Layout of a Process

The memory allocated to each processis composed of a number of parts, usually referred to as *segments*. These segments are as follows:

* text segment
* initialized data segment
* uninitialized data segment(bss)
* stack
* heap

## Virtual Memory Management

Like most modern kernels, Linux employs a technique known as `virtual memory management`. Most programs demonstrate two kinds of localityL

* Spatial locality
* Temporal locality

A virutal memory scheme splits the memory used by each program into small, fixed-size units called `pages`. Correspondingly, RAM is divided into a series of `page frames` of the same size.


## The Stack and Stack Frames

The stack grows and shrinks linearly as functions are called and return. For Linux on teh x86-32 architecture, the stack resides at the high end of memory and grows downward(toward the heap).

A special-purpose register, the `stack pointer`, tracks the current top of the stack. Each time a function is called, an additional frame is allocated on the stack and this frame is removed when the function returns.

* *kernel stack* is a per-process memory region maintained in kernel memory that is used as the stack for execution of the functions called internally during the execution of a system call.

Each *user stack frame* containes the following information:

* Functions arguments and local variables:
* Call linkage information


## Command line Arguments

Every C program must have a function called `main()`, which is the point where execution of the program starts.

* int argc
* char **argv

## Environment List

Each process has an associated array of strings called the `environment list` or simply the `environment`. Each of these strings is a definition of the form `name=value`.

When a new process is created, it inherits a copy of its parent's environment.

A common use of environment variables is in the shell, By placing values in its own environment. the shell can ensure that these values are passed to the processes that it creates to execute user commands.

Some library functions allow their behavior to be modified by setting environment variables. This allows the user to control the behavior of an application using the function without needing to change the code of the application or relink it asgainst the corresponding library.

Within a C program, the environment list can be accessed using the global variable `char **environ`. An alternative method of accessing the environment list is to declare a third argument to the `main()` funciton:

```
	int main(int argc, char *argv[], char *envp[]
```

## Performing a Nonlocal goto: setjmp() and longjmp()

The `setjmp()` and `longjmp()` library functions are used to perform a nonlocal goto. Nonlocal gotos can render a program difficult to read and maintain, and should be avoided whenever possible.
