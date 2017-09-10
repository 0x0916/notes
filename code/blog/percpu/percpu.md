## percpu

说起percpu时，我们先学习一下`kernel 2.4`中使用的**每个cpu的分配**机制。


### 每个cpu的分配

支持SMP的现代操作系统使用每个cpu上的数据，对于给定的处理器其数据是唯一的。所以，么个cpu的数据存放在一个数组中，数组中的每一项对应着系统上的一个存在的处理器。

使用时，可以声明数据结构如下：

```c
unsigned long my_percpu[NR_CPUS];
```

然后，按照如下方式访问它：

```c
int cpu;

cpu = get_cpu();
my_percpu[cpu]++;
printk("my_percpu on cpu= %d is %lu\n", cpu, my_percpu[cpu]);
put_cpu();
```

上面的代码并没有出现锁，以为所操作的数据对当前处理器时唯一的。除了当前处理器外，没有其他处理器可以接触到这个数据，不存在并发访问的问题。

所以，内核抢占成了唯一需要关心的问题，因为内核抢占会引起如下两个问题：

1. 如果代码被其他处理器抢占并重新调度，那么这时cpu变量就会无效。因为它指向了错误的处理器。
2. 如果另外一个任务抢占了你的代码，那么可能在同一个处理器上并发访问my_percpu的情况，这属于一个竞争条件。


其实这些都不用担心，因为get_cpu()时，已经禁止了内核抢占，而put_cpu()时，又会重新激活当前处理器。其代码如下：


```c
#define get_cpu()		({ preempt_disable(); smp_processor_id(); })
#define put_cpu()		preempt_enable()
```

### 新的每个CPU接口


2.6 内核为了方便的创建和操作每个cpu数据，引入了新的接口，称作percpu。该接口归纳了前面所述的操作行为，简化了创建和操作每个cpu的数据。

#### 编译时的每个cpu数据

定义每个cpu变量

```c
DEFINE_PER_CPU(type, name)
```

该语句位系统中的每一个处理器都定义了一个类型为`type`，名字为`name`的变量。

如果要在其他文件中声明变量，可以使用如下宏：

```c
DECLARE_PER_CPU(type, name)
```

操作变量可以使用如下宏`get_cpu_var(var)`和`put_cpu_var(var)`，同样的，这两个宏分别禁止内核抢占和重新激活内核抢占。

```c
/*
 * Must be an lvalue. Since @var must be a simple identifier,
 * we force a syntax error here if it isn't.
 */
#define get_cpu_var(var) (*({				\
	preempt_disable();				\
	&__get_cpu_var(var); }))

/*
 * The weird & is necessary because sparse considers (void)(var) to be
 * a direct dereference of percpu variable (var).
 */
#define put_cpu_var(var) do {				\
	(void)&(var);					\
	preempt_enable();				\
} while (0)
```

使用方法如下：

```c
get_cpu_var(name)++;
put_cpu_var(name);
```

同时，也可以获取别的处理器上的每个cpu数据：

```c
per_cpu(name, cpu)++;
```

> 注意：per_cpu并不会禁止内核抢占，也不会提供任何形式的锁机制，着需要用户自己去保证数据的访问安全。



#### 运行时的每个cpu数据


内核实现`每个cpu数据`的动态分配方法类似与`kmalloc()`，该例程为系统上每个处理器创建所需内存的实例。


```c
void __percpu *__alloc_percpu(size_t size, size_t align);
void free_percpu(void __percpu *__pdata
#define alloc_percpu(type)	\
	(typeof(type) __percpu *)__alloc_percpu(sizeof(type), __alignof__(type))
```


无论是`alloc_percpu`还是`__alloc_percpu`都会返回一个指针，它用来间接引用动态创建的每个cpu数据。内核提供了如下两个宏来利用指针获取每个cpu的数据:

* get_cpu_var(ptr)
* put_cpu_var(ptr)

`get_cpu_var`宏返回了一个指向当前处理器数据的特殊实例，它同时会禁止内核抢占；而在`put_cpu_var`宏中会重新激活内核抢占。


一个完整的例子：

```c

void *percpu_ptr;
unsigned long *foo;
percpu_ptr = alloc_percpu(unsigned long);
if (!ptr)
	/*内存分配错误*/

foo = get_cpu_var(percpu_ptr);
/*操作foo*/
put_cpu_var(percpu_ptr);
```

### 使用每个CPU数据的原因

使用每个cpu数据具有不少好处：

* 首先减少了数据锁定
* 每个cpu数据可以大大减少缓存失效


总结：使用每个cpu数据会省去许多（或最小化）数据上锁，它唯一的安全要求就是**要禁止内核抢占**。







