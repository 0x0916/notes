package main

import "fmt"

// `(int, int)` 表明该函数返回两个类型为int的值
func numbers() (int, int) {
	return 2, 6
}

func main() {
	// 这里我们使用两个不同的值来存储函数返回的值
	a, b := numbers()
	fmt.Println(a)
	fmt.Println(b)

	// 如果我们只关心其中的某个返回值，可以用空白定义符`_`标识不关心的值
	_, c := numbers()
	fmt.Println(c)
}
