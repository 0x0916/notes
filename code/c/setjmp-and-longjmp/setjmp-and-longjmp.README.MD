## setjmp and longjmp

`setjmp` 和 `longjmp` 一起可以实现函数间的跳转。这两个函数对于处理发生在深层次嵌套错误的场景中是非常有用的。
同时，结合`clone`系统调用，还可以实现特殊的代码流程控制。

但请注意该函数跳转后，对自动变量、寄存器变量、易失变量的影响。




