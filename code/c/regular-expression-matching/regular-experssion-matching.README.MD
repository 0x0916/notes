`GNU C` 支持两种匹配正则表达式的接口，一种为标准的`POSIX.2`接口，另外一种存在于`GNU C`中很多年了。这两种接口都在头文件`regex.h`中声明，如果你定义了`_POSIX_C_SOURCE`,则只会声明标准的`POSIX.2`接口。

C语言处理正在表达式常用的函数有`regcomp()`,`regexec()`,`regfree()`, `regfree()`,一般分为三个步骤，如下所示：

* 编译正则表达式`regcomp`
* 匹配正则表达式`regexec`
* 释放正则表达式`regfree`


## 编译正则表达式

```c
	int regcomp (regex_t *compiled, const char *pattern, int cflags);
```

这个函数把指定的正则表达式`pattern`编译成一种特定的数据格式`compiled`，这样可以使匹配更有效。


参数说明：

* `regex_t` 是一个结构体数据类型，用来存放编译后的正则表达式，它的成员`re_nsub` 用来存储正则表达式中的子正则表达式的个数，子正则表达式就是用圆括号包起来的部分表达式。
* `pattern` 是指向我们写好的正则表达式的指针。
* `cflags` 有如下4个值或者是它们或运算`(|)`后的值：
- `REG_EXTENDED` 以功能更加强大的扩展正则表达式的方式进行匹配。
- `REG_ICASE` 匹配字母时忽略大小写。
- `REG_NOSUB` 不用存储匹配后的结果。
- `REG_NEWLINE` 识别换行符，这样`$`就可以从行尾开始匹配，`^`就可以从行的开头开始匹配。

## 匹配正则表达式

```c
	int regexec (regex_t *compiled, char *string, size_t nmatch, regmatch_t matchptr [], int eflags);
```

当我们编译好正则表达式后，就可以用`regexec` 匹配我们的目标文本串了，如果在编译正则表达式的时候没有指定`cflags`的参数为`REG_NEWLINE`，则默认情况下是忽略换行符的，也就是把整个文本串当作一个字符串处理。


`regmatch_t`是一个结构体数据类型，在`regex.h`中定义:

```c
typedef struct
{
   regoff_t rm_so;
   regoff_t rm_eo;
} regmatch_t;
```

成员`rm_so` 存放匹配文本串在目标串中的开始位置，`rm_eo` 存放结束位置。通常我们以数组的形式定义一组这样的结构。因为往往我们的正则表达式中还包含子正则表达式。数组0单元存放主正则表达式位置，后边的单元依次存放子正则表达式位置。

参数说明：
* `compiled` 是已经用`regcomp`函数编译好的正则表达式。
* `string` 是目标文本串。
* `nmatch` 是`regmatch_t`结构体数组的长度。
* `matchptr` `regmatch_t`类型的结构体数组，存放匹配文本串的位置信息。
* `eflags` 有两个值

- `REG_NOTBOL` 让特殊字符`^`无作用
- `REG_NOTEOL` 让特殊字符`$`无作用

## 释放正则表达式

```c
void regfree (regex_t *compiled);
```

当我们使用完编译好的正则表达式后，或者要重新编译其他正则表达式的时候，我们可以用这个函数清空`compiled`指向的`regex_t`结构体的内容，请记住，如果是重新编译的话，一定要先清空`regex_t`结构体。


## 返回`regcomp`,`regexec`的错误原因

```c
size_t regerror (int errcode, regex_t *compiled, char *buffer, size_t length);
```

当执行`regcomp` 或者`regexec` 产生错误的时候，就可以调用这个函数而返回一个包含错误信息的字符串。

参数说明：
* `errcode` 是由`regcomp` 和 `regexec` 函数返回的错误代号。
* `compiled` 是已经用`regcomp`函数编译好的正则表达式，这个值可以为`NULL`。
* `buffer` 指向用来存放错误信息的字符串的内存空间。
* `length` 指明`buffer`的长度，如果这个错误信息的长度大于这个值，则`regerror` 函数会自动截断超出的字符串，但他仍然会返回完整的字符串的长度。所以我们可以用如下的方法先得到错误字符串的长度。

```c
size_t length = regerror (errcode, compiled, NULL, 0);
```

## 示例1

一个简单的匹配email的例子

## 示例2

从命令行获取正则表达式，然后将其运用于从标准输入得到的每行数据，并打印出匹配结果。

