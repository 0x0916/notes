## seq_file

一般地，内核通过在`procfs`文件系统下建立文件来向用户空间提供输出信息，用户空间可以通过任何文本阅读应用查看该文件信息，但是`procfs `有一个缺陷，如果输出内容大于1个内存页，需要多次读，因此处理起来很难，另外，如果输出太大，速度比较慢，有时会出现一些意想不到的情况， `Alexander Viro`实现了一套新的功能，使得内核输出大文件信息更容易，该功能出现在`2.4.15`（包括2.4.15）以后的所有2.4内核以及2.6内核中，尤其 是在2.6内核中，已经大量地使用了该功能。


要想使用`seq_file`功能，开发者需要包含头文件`linux/seq_file.h`，关于`seq_file`主要有以下三个方面需要关注：

* 一个迭代器接口，用来循环输出指定的数据对象
* 一些工具函数，用来格式化输出
* 一些已经定义好的`file_operations`，用来简化操作

### 迭代器接口

迭代器接口需要定义如下四个函数，并将其设置到一个`seq_operations`结构中：

```c
struct seq_operations {
	void * (*start) (struct seq_file *m, loff_t *pos);
	void (*stop) (struct seq_file *m, void *v);
	void * (*next) (struct seq_file *m, void *v, loff_t *pos);
	int (*show) (struct seq_file *m, void *v);
};
```

* `start`函数用于指定seq_file文件的读开始位置，返回实际读开始位置，如果指定的位置超过文件末尾，应当返回NULL，start函数可以有一个特殊的返回SEQ_START_TOKEN，它用于让show函数输出文件头，但这只能在pos为0时使用；
* `next`函数用于把seq_file文件的当前读位置移动到下一个读位置，返回实际的下一个读位置，如果已经到达文件末尾，返回NULL；
* `stop`函数用于在读完seq_file文件后调用，它类似于文件操作close，用于做一些必要的清理，如释放内存等；
* `show`函数用于格式化输出，如果成功返回0，否则返回出错码。

### 格式化辅助函数

`seq_file`也定义了一些辅助函数用于格式化输出：


```c
/*函数seq_putc用于把一个字符输出到seq_file文件*/
int seq_putc(struct seq_file *m, char c);
  
/*函数seq_puts则用于把一个字符串输出到seq_file文件*/
int seq_puts(struct seq_file *m, const char *s);

/*函数seq_write用于将二进制数据输出到seq_file文件*/
int seq_write(struct seq_file *seq, const void *data, size_t len);

/*函数seq_escape类似于seq_puts，只是，它将把第一个字符串参数中出现的包含在第二个字符串参数中的字符按照八进制形式输出，也即对这些字符进行转义处理*/
int seq_escape(struct seq_file *, const char *, const char *);

/*函数seq_printf是最常用的输出函数，它用于把给定参数按照给定的格式输出到seq_file文件*/
__printf(2, 3) int seq_printf(struct seq_file *, const char *, ...);
__printf(2, 0) int seq_vprintf(struct seq_file *, const char *, va_list args);

/*此外还有如下辅助函数*/
int seq_path(struct seq_file *, const struct path *, const char *);
int seq_dentry(struct seq_file *, struct dentry *, const char *);
int seq_path_root(struct seq_file *m, const struct path *path,
		  const struct path *root, const char *esc);
int seq_bitmap(struct seq_file *m, const unsigned long *bits,
				   unsigned int nr_bits);
```

### 其他已经定义好的`file_operations`

在定义了结构`struct seq_operations`之后，用户还需要把打开`seq_file`文件的`open`函数，以便该结构与对应于`seq_file`文件的`struct file`结构关联起来，例如，`struct seq_operations`定义为：

```c
struct seq_operations ct_file_ops = {
     .start = ct_seq_start,
     .stop = ct_seq_stop,
     .next = ct_seq_next,
     .show = ct_seq_show
};
  ```

那么，`open`函数应该如下定义：

```c
static int ct_seq_open(struct inode *inode, struct file *file)
{
          return seq_open(file, &exam_seq_ops);
};
```

> 函数`seq_open`是`seq_file`提供的函数，它用于把`struct seq_operations`结构与`seq_file文件`关联起来

最后，用户需要如下设置`struct file_operations`结构：

```c
struct  file_operations ct_file_ops = {
          .owner   = THIS_MODULE,
          .open    = ct_seq_open,
          .read    = seq_read,
          .llseek  = seq_lseek,
          .release = seq_release
};
```
> 用户仅需要设置open函数，其它的都是seq_file提供的函数。

然后，用户创建一个`/proc`文件并把它的文件操作设置为`ct_file_ops`即可：

```c
proc_create("sequence", 0, NULL, &ct_file_ops);
```

### seq_list

如果文件输出的数据来自于一个链表，如下的辅助函数会非常有用：

```c
/*
 * Helpers for iteration over list_head-s in seq_files
 */

extern struct list_head *seq_list_start(struct list_head *head,
		loff_t pos);
extern struct list_head *seq_list_start_head(struct list_head *head,
		loff_t pos);
extern struct list_head *seq_list_next(void *v, struct list_head *head,
		loff_t *ppos);

/*
 * Helpers for iteration over hlist_head-s in seq_files
 */

extern struct hlist_node *seq_hlist_start(struct hlist_head *head,
					  loff_t pos);
extern struct hlist_node *seq_hlist_start_head(struct hlist_head *head,
					       loff_t pos);
extern struct hlist_node *seq_hlist_next(void *v, struct hlist_head *head,
					 loff_t *ppos);

extern struct hlist_node *seq_hlist_start_rcu(struct hlist_head *head,
					      loff_t pos);
extern struct hlist_node *seq_hlist_start_head_rcu(struct hlist_head *head,
						   loff_t pos);
extern struct hlist_node *seq_hlist_next_rcu(void *v,
						   struct hlist_head *head,
						   loff_t *ppos);

/* Helpers for iterating over per-cpu hlist_head-s in seq_files */
extern struct hlist_node *seq_hlist_start_percpu(struct hlist_head __percpu *head, int *cpu, loff_t pos);

extern struct hlist_node *seq_hlist_next_percpu(void *v, struct hlist_head __percpu *head, int *cpu, loff_t *pos);

#endif

```

### 只定义show函数

对于简单的输出，`seq_file`用户并不需要定义和设置这么多函数与结构，它仅需定义一个`show`函数，然后使用`single_open`来定义`open`函数就可以，以下是使用这种简单形式的一般步骤：

定义`show`函数

 ```c
 int ct_show(struct seq_file *p, void *v)
 {
 
 }
 ```
 
 定义`open`函数
 
 ```c
 int  ct_single_open(struct inode *inode, struct file *file)
 {
          return(single_open(file, ct_show,  NULL));
 }  
 ```
 > 注意：要使用single_open而不是seq_open。
 
 定义`struct file_operations`结构  
 
 ```c
 struct file_operations ct_single_seq_file_operations = {
          .open           = ct_single_open,
          .read           = seq_read,
          .llseek         = seq_lseek,
          .release        = single_release,
};
```
> 注意，如果`open`函数使用了`single_open`，`release`函数必须为`single_release`，而不是`seq_release`。


### 示例

该模块在`/proc`下创建了文件`mytask`s，它输出了当前系统上所有的进程
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>


static void *tasks_seq_start(struct seq_file *s, loff_t *pos)
{
	if (*pos == 0) {
		seq_printf(s, "All processes in system:\n%-24s%-5s\n", "name", "pid");
		return &init_task;
	} else {
		return NULL;
	}
}

static void *tasks_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct task_struct *task = (struct task_struct *)v;

	task = next_task(task);
	if ((*pos != 0) && (task == &init_task)) {
		return NULL;
	}
	++*pos;
	return task;
}

static void tasks_seq_stop(struct seq_file *s, void *v)
{
}

/*
 * The show function.
 */
static int tasks_seq_show(struct seq_file *s, void *v)
{
	struct task_struct *task = (struct task_struct *)v;

	seq_printf(s, "%-24s%-5d\n", task->comm, task->pid);

	return 0;
}

static struct seq_operations tasks_seq_ops = {
	.start = tasks_seq_start,
	.next = tasks_seq_next,
	.stop = tasks_seq_stop,
	.show = tasks_seq_show
};

static int tasks_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &tasks_seq_ops);
}

static struct file_operations tasks_file_ops = {
	.owner   = THIS_MODULE,
	.open    = tasks_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

static int __init proc_mytasks_init(void)
{
	struct proc_dir_entry *pe;
	pe = proc_create("mytasks", 0, NULL, &tasks_file_ops);
	if (pe == NULL) {
		pr_err("proc_create failed\n");
		return -1;
	}
	return 0;
}

static void __exit proc_mytasks_exit(void)
{
	remove_proc_entry("mytasks", NULL);
}

module_init(proc_mytasks_init);
module_exit(proc_mytasks_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("datawolf");

```





 
 