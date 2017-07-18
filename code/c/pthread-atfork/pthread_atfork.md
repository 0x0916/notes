
`pthread_atfork` 结合 `mutex`时，在子进程中`unlock`会报错或者行为未定义。具体参考如下链接：

* https://stackoverflow.com/questions/2620313/how-to-use-pthread-atfork-and-pthread-once-to-reinitialize-mutexes-in-child?answertab=votes#tab-top
* http://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
