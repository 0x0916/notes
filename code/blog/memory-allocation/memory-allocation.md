## Allocating Memory on the Heap

A process can allocate memroy by increasing the size of the heap, a variablesize segment of contiguous virtual memroy that begins just after the uninitialized data segment of a process and grows and shrinks as memory is allocated and freed.

The current limit of the heap is referred to as the *program break*.

### adjusting the Program Break: brk() and sbrk()

```
#include <unistd.h>

int brk(void *end_data_segment);

void *sbrk(intptr_t increment);
```

The *brk()* system call sets the program break to the location specified by *end_data_segment*.

A call to *sbrk()* adjust the program break by adding *increment* to it.(On Linux, *sbrk()* is a library function implemented on top of *brk()*)

The call *sbrk(0)* returns the current settting of the program break without changing it. This can be useful if we want to brack the size of the heap.

### Allocating Memory on the Heap: malloc() and free()

In general, C programs use the *malloc* family of functions to allocate and deallocate memory on the heap.

The *malloc()* function allocated size bytes from the heap and returns a pointer to the start of the newly allocated block of memory.(The allocated memory is not initialized)

The *free()* function deallocates the block of memory pointer to by its *ptr* argument, which should be an address previously returned by *malloc()*.

In general, *free()* does not lower the program break, but instead adds the block of memroy to a list of free block that are recycled by future calls to malloc().

If the argument given to *free()* is a NULL pointer, then the call does nothing.(In other words, it is not an error to give a NULL pointer to *free()*)

#### Example program

### Implementation of *malloc()* and *free()*

The implementation of *malloc()* is straightforward. It first scans the list of memory blocks previously released by *free()* in order to find one whose size if larger than or equal to its requirements.(Different strategies may be employed for this scan, for example, first-fit or best-fit).

* If the block is exactly the right size, then it is returned to the caller.
* If it is larger, then it is split, so that a block of the correct size is returned to the caller and a smaller free block is left on the free list.
* If no block on the free list is large enouggh, then *malloc()* calls *sbrk()* to allocate more memory.(一般情况下，为了减少*sbrk()*的调用次数，*malloc()*给*program break*增加一个比申请内存大的一个值，然后将剩余的添加到空闲列表中。)


When *malloc()* allocates the block, it allocates extra bytes to hold an integer containing the size of the block. This integer is located at the beginning of the block; the address actually returned to caller points to the location just past this length value.

```
_________________________________________________________
|   Length of  block   |       memory for use by caller |
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                       |
                       address returned by malloc()
```

When a block is placed on the (doubly linked) free list, *free()* uses the bytes of the block itself in order to add the block to the list.

为了避免各种内存错误，我们应该遵循如下的原则：

* after we allocate a block of memory, we should be careful not to touch any bytes outside the range of that block.
* It is an error to free the same piece of allocated memory more than once.
* we should never call *free()* with pointer value that was not obtained by a call to one of the functions in the *malloc* package.
* If we are writing a long-running programe that repeatedly allocates memory for various purposes, then we should ensure that we deallocated any memory after we have finished using it.(avoid *memory leak*)

#### Tools and libraries for *malloc* debugging

内存申请、释放和使用会遇到一些很难复现的错误，我们可以使用一些debug工具来提前发现这些错误。

* The *mtrace()* and  *muntrace()* functions allows a program to turn tracing of memory allocation calls on and off.
* The *mcheck()* and *mprobe()* functions allows a program to perform consistency check on blocks of allocated memory.
* The *MALLOC_CHECK_* environment variable serves a similar purpose to *mcheck()* and *mprobe()*.(优势是：不需要重新编译程序) Possible settings are:
  - 0, meaning ignore errors;
  - 1, meaning print diagnostic errors on stderr;
  - 2, meaning call abort() to terminate the program.

##### Example

```
$ cat t_mtrace.c
#include <mcheck.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int j;

	mtrace();

	for( j= 0; j < 2; j++)
		malloc(100);	/* never freed ---- a memory leak */

	calloc(16, 16);		/* never freed ---- a memory leak */
	exit(EXIT_SUCCESS);
}
```

按照如下步骤运行程序，我们可以看到检查出了内存泄漏：

```
$ gcc -g t_mtrace.c -o t_mtrace
$ export MALLOC_TRACE=/tmp/t
$ ./t_mtrace
$ mtrace ./t_mtrace $MALLOC_TRACE

Memory not freed:
-----------------
           Address     Size     Caller
0x000000000068d450     0x64  at /go/src/github.com/0x0916/notes/code/blog/memory-allocation/t_mtrace.c:10 (discriminator 3)
0x000000000068d4c0     0x64  at /go/src/github.com/0x0916/notes/code/blog/memory-allocation/t_mtrace.c:10 (discriminator 3)
0x000000000068d530    0x100  at /go/src/github.com/0x0916/notes/code/blog/memory-allocation/t_mtrace.c:14

```
此外，还有一些调试工具来发现内存错误，比如：Valgrind。(we should use them only for debugging purposes, because we need to link oure application against that library instead of the malloc package in the standard C library and these libraries typically operate at the cost of slower run-time operation, increased memory consumption, or both.)

#### Controlling and monitoring the *malloc* package

* The *mallopt()* function modifies various parameters that control the algorithm used by *malloc()*.
* The *mallinfo()* function returns a structure containing various statistics about the memory allocated by *malloc()*.



### Other Methods of Allocating Memory on the Heap

#### Allocating memory with *calloc()* and *realloc()*

* The *calloc()* function allocates memory for an array of identical items. Unlike *malloc()*, *calloc()* initializes the allocated memory to 0.
* The *realloc()* function is used to resize (usually enlarge) a block of memory previously allocated by one of the functions in the *malloc* package. When *relloc()* increases the size of a block of allocated memory, it does not initialize the additionally allocated bytes.


Memory allocated using *calloc()* or *realloc()* should be deallocated with *free()*. In general, it is advisable to minimize the use of *realloc()*.

#### Allocating aligned memory: *memalign()* and *posix_memalign()*

The *memalign()* and *posix_memalign()* functions are designed to allocate memory starting at an address aligned at a specified power-of-two boundary.


## Allocating Memory on the Stack: *alloca()*

*alloca()* obtains memory from the stack by increasing the size of the stack frame. We need not (indeed, must not) call *free()* to deallocate memory allocated with *alloca()*. Likewise, it is not possible to use *realloc()* to resize a block of memory allocated by *alloca()*.

If the stack overlows as a consequence of calling *alloca()*, then program behavior is unpredictable. Note that we can not use *alloca()* within a function argument list.

Using *alloca()* to allocate memory has a few advantages over *malloc()*:

* allocating blocks of memory is faster with *alloca()* than with *malloc()*.
* the memory that is allocates is automatically freed when the stack fram is removed.

Using *alloca()* can be especialy usefull if we employ *longjmp()* or *siglongjmp()* to perform a nonlocal goto from a signal handler.

