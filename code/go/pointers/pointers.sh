# `zeroval`在`main`中不能改变`a`的值，但是`zeroptr`可以。
$ go run pointers.go 
initial: 1
zeroval: 1
initial: 1
zeroptr: 0
ptr: 0xc42000a2f8
