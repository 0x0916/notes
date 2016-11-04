Go语言提供的包`expvar`提供了一个标准的接口用来导出变量，它将这些变量通过HTTP以JSON的格式导出。

设置和修改这些导出变量的操作都是原子的。

该包默认注册如下两个变量：

* cmdline   os.Args
* memstats  runtime.Memstats
