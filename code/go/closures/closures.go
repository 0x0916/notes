package main

import "fmt"

// 这个`intSeq`函数的返回值是一个`func() int`类型的函数
// 返回的函数定义在`intSeq`函数体内的匿名函数。这个返回的函数
// 使用闭包的方式隐藏变量`i`
func intSeq() func() int {
	i := 0
	return func() int {
		i += 1
		return i
	}
}

func main() {
	// 调用`intSeq`函数，将返回值（也是一个函数）赋值给`nextInt`。
	// 这个函数的值包含了自己的值`i`，这样每次调用`nextInt`时都会更新`i`的值。
	nextInt := intSeq()

	// 通过多次调用`nextInt`来看看闭包的效果
	fmt.Println(nextInt())
	fmt.Println(nextInt())
	fmt.Println(nextInt())
	fmt.Println(nextInt())

	// 为了确定这个状态对于这个特定的函数是唯一的，我们重新创建并测试一下。
	newNextInt := intSeq()
	fmt.Println(newNextInt())
}
