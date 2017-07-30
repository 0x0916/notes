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

### Example program













