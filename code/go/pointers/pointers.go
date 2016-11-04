package main

import "fmt"

// `zeroval`使用值传递
func zeroval(ival int) {
	ival = 0
}

// `zeroptr`使用指针传递，对一个解引用的指针赋值会改变这个指针引用的真实地址的值
func zeroptr(iptr *int) {
	*iptr = 0
}

func main() {
	a := 1
	fmt.Println("initial:", a)
	zeroval(a)
	fmt.Println("zeroval:", a)
	b := 1

	fmt.Println("initial:", b)
	zeroptr(&b)
	fmt.Println("zeroptr:", b)
	fmt.Println("ptr:", &b)
}
