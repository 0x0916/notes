在编写应用程序时，为了方便使用内核的`syscall filering mechanism`，我们可以使用`libseccomp`，它提供了一个平台无关的，简单易用的接口。


* [seccomp_version][seccomp_version] 可以用来判断`libseccomp`库的版本。
* `seccomp_init` 用来初始化seccomp filter的状态
* `seccomp_reset` 用来重新初始化seccomp filter的状态
* `seccomp_release`用来释放存在的seccomp filter context
* `seccomp_load` Load the current seccomp filter into the kernel
* `seccomp_rule_add`, `seccomp_rule_add_exact`, `seccomp_rule_add_array`, `seccomp_rule_add_exact_array` Add a seccomp filter rule
* `seccomp_merge` Merge two seccomp filters
* `seccomp_attr_set`, `seccomp_attr_get` - Manage the seccomp filter attributes
* `seccomp_arch_add`, `seccomp_arch_remove`, `seccomp_arch_exist`, `seccomp_arch_native` - Manage seccomp filter architectures
* `seccomp_syscall_resolve_name`,`seccomp_syscall_resolve_name_arch`, `seccomp_syscall_resolve_name_rewrite`,`seccomp_syscall_resolve_num_arch` - Resolve a syscall name, 利用这些函数，我们可以编写出一个简单的小工具，用来根据系统调用函数名称查询系统调用号[syscall][syscall_resolver].
* `seccomp_syscall_priority` Prioritize syscalls in the seccomp filter
* `seccomp_export_bpf`, `seccomp_export_pfc` - Export the seccomp filter



[seccomp_version]: https://github.com/0x0916/notes/tree/master/code/c/libseccomp/seccomp_version
[syscall_resolver]: https://github.com/0x0916/notes/tree/master/code/c/libseccomp/syscall_resolver
