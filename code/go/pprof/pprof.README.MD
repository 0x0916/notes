在Go程序中加入如下代码

```
import _ "net/http/pprof"
```

会安装在URL `/debug/pprof` 上导出`profileing data`
