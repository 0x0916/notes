package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	// Go 通过向一个`Channel`发送`os.Signal`值来进行信号的通知。
	// 我们创建一个`Channel`来接收这些信号。
	sigs := make(chan os.Signal, 1)

	// 创建一个用于程序结束时进行通知的Channel
	done := make(chan bool, 1)

	// `singal.Notify` 注册给定的`Channel`，用于接收特定的信号
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	// 这个Go协程执行一个阻塞的信号接收操作。当它得到一个值时，
	// 它将打印这个值，然后通知程序可以退出。
	go func() {
		sig := <-sigs // block until a signal is received.
		fmt.Println()
		fmt.Println(sig)
		done <- true
	}()

	// 程序将在这里进行等待，知道它达到了期望的信号（也就是上面的Go协程发送的`done`值），然后退出。
	fmt.Println("awaiting signal")
	<-done
	fmt.Println("exiting")
}
