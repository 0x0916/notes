## initgroups



`initgroups`函数的原型为：

```
	int initgroups(const char *user, gid_t group);
```

其作用是初始化当前进程的可访问组列表为用户`user`为成员的所有组，此外参数`group`指定的组也会被添加到该列表中。

