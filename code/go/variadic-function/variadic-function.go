package main

import "fmt"

// 该函数使用任意数量的`int`类型的参数
func sum(nums ...int) {
	fmt.Println(nums, "")
	total := 0
	for _, num := range nums {
		total += num
	}
	fmt.Println(total)
}

func main() {
	// 变参函数使用常规的调用方式。
	sum(1, 2)
	sum(1, 2, 3)

	// 如果`slice`中已经有了多个值，想要把他们变成参数使用，需要
	// 这样来调用`func(slice...)`
	nums := []int{1, 2, 3, 4}
	sum(nums...)
}
