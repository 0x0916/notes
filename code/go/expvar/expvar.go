package main

import (
	"expvar"
	"fmt"
	"net/http"
)

// 定义两个指标，`numCalls` 统计访问我们服务器数量，
// `lastUser`记录最后一个访问用户的名称
var numCalls = expvar.NewInt("num_calls")
var lastUser = expvar.NewString("last_user")

func HelloServer(w http.ResponseWriter, req *http.Request) {
	user := req.FormValue("user")

	// 更新指标
	numCalls.Add(1)
	lastUser.Set(user)

	msg := fmt.Sprintf("G'day %s\n", user)
	w.Write([]byte(msg))
}

func main() {
	http.HandleFunc("/", HelloServer)
	http.ListenAndServe(":4545", nil)
}
