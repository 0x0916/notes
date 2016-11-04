package main

import (
	"log"
	"net/http"
	_ "net/http/pprof"
)

func main() {
	// 在4546端口上启动一个HTTP服务
	log.Println(http.ListenAndServe(":4546", nil))
}
