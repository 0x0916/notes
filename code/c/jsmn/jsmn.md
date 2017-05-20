在资源非常受限的计算机系统上，解析`JSON`时，我们需要一个不依赖于第三方库的，非常简单的解析方法，通过搜索
找到了一个名称为`JSMN`的程序。

该程序非常简单，其主页地址为[http://zserge.com/jsmn.html](http://zserge.com/jsmn.html)。其特点如下

* simple
* higly portable
* no dependencies(even libc!)
* no dynamic memory allocation
* extremely small code footprint
* API has only 2 functions
* incremental single-pass parsing

正是由于其非常简单，只有两个`API`，所以使用起来得花费一些时间。本文就简单介绍一下几个示例用法。

以下示例中，我们使用如下的`json`字符串。

```json
{
  "defaultAction": "SCMP_ACT_ERRNO",
  "architectures": [
    "SCMP_ARCH_X86_64",
    "SCMP_ARCH_X86",
    "SCMP_ARCH_X32"
  ],
  "syscalls": [
    {
      "name": "write",
      "action": "SCMP_ACT_ALLOW"
    },
    {
      "name": "socketcall",
      "action": "SCMP_ACT_ALLOW",
      "args": [
        {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        },
        {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
      ]
    }
  ]
}
```

## 示例0: JSMN解除后的数据结构

上面的`json`字符串[解析完成][jsmn-internal]，其在`JSMN`的内部表示如下：

```
[0] size = 3, type = object
[0] content = {
  "defaultAction": "SCMP_ACT_ERRNO",
  "architectures": [
    "SCMP_ARCH_X86_64",
    "SCMP_ARCH_X86",
    "SCMP_ARCH_X32"
  ],
  "syscalls": [
    {
      "name": "write",
      "action": "SCMP_ACT_ALLOW"
    },
    {
      "name": "socketcall",
      "action": "SCMP_ACT_ALLOW",
      "args": [
        {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        },
        {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
      ]
    }
  ]
}
[1] size = 1, type = string
[1] content = defaultAction
[2] size = 0, type = string
[2] content = SCMP_ACT_ERRNO
[3] size = 1, type = string
[3] content = architectures
[4] size = 3, type = array
[4] content = [
    "SCMP_ARCH_X86_64",
    "SCMP_ARCH_X86",
    "SCMP_ARCH_X32"
  ]
[5] size = 0, type = string
[5] content = SCMP_ARCH_X86_64
[6] size = 0, type = string
[6] content = SCMP_ARCH_X86
[7] size = 0, type = string
[7] content = SCMP_ARCH_X32
[8] size = 1, type = string
[8] content = syscalls
[9] size = 2, type = array
[9] content = [
    {
      "name": "write",
      "action": "SCMP_ACT_ALLOW"
    },
    {
      "name": "socketcall",
      "action": "SCMP_ACT_ALLOW",
      "args": [
        {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        },
        {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
      ]
    }
  ]
[10] size = 2, type = object
[10] content = {
      "name": "write",
      "action": "SCMP_ACT_ALLOW"
    }
[11] size = 1, type = string
[11] content = name
[12] size = 0, type = string
[12] content = write
[13] size = 1, type = string
[13] content = action
[14] size = 0, type = string
[14] content = SCMP_ACT_ALLOW
[15] size = 3, type = object
[15] content = {
      "name": "socketcall",
      "action": "SCMP_ACT_ALLOW",
      "args": [
        {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        },
        {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
      ]
    }
[16] size = 1, type = string
[16] content = name
[17] size = 0, type = string
[17] content = socketcall
[18] size = 1, type = string
[18] content = action
[19] size = 0, type = string
[19] content = SCMP_ACT_ALLOW
[20] size = 1, type = string
[20] content = args
[21] size = 2, type = array
[21] content = [
        {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        },
        {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
      ]
[22] size = 4, type = object
[22] content = {
          "index": 0,
          "value": 1,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
[23] size = 1, type = string
[23] content = index
[24] size = 0, type = primitive
[24] content = 0
[25] size = 1, type = string
[25] content = value
[26] size = 0, type = primitive
[26] content = 1
[27] size = 1, type = string
[27] content = valueTwo
[28] size = 0, type = primitive
[28] content = 0
[29] size = 1, type = string
[29] content = op
[30] size = 0, type = string
[30] content = SCMP_CMP_EQ
[31] size = 4, type = object
[31] content = {
          "index": 1,
          "value": 17,
          "valueTwo": 0,
          "op": "SCMP_CMP_EQ"
        }
[32] size = 1, type = string
[32] content = index
[33] size = 0, type = primitive
[33] content = 1
[34] size = 1, type = string
[34] content = value
[35] size = 0, type = primitive
[35] content = 17
[36] size = 1, type = string
[36] content = valueTwo
[37] size = 0, type = primitive
[37] content = 0
[38] size = 1, type = string
[38] content = op
[39] size = 0, type = string
[39] content = SCMP_CMP_EQ
```

## 示例1: Dump `json`

以上`json`字符串[dump的结果][jsmn-dump]如下：

```
'defaultAction': 'SCMP_ACT_ERRNO'
'architectures':
   - 'SCMP_ARCH_X86_64'
   - 'SCMP_ARCH_X86'
   - 'SCMP_ARCH_X32'

'syscalls':
   -
    'name': 'write'
    'action': 'SCMP_ACT_ALLOW'

   -
    'name': 'socketcall'
    'action': 'SCMP_ACT_ALLOW'
    'args':
       -
        'index': 0
        'value': 1
        'valueTwo': 0
        'op': 'SCMP_CMP_EQ'

       -
        'index': 1
        'value': 17
        'valueTwo': 0
        'op': 'SCMP_CMP_EQ'
```

## 示例2:

对于我们已知的JSON数据结构，我们可以使用状态机进行解析。

## 示例3:
## 示例4:

[jsmn-internal]: https://github.com/0x0916/notes/tree/master/code/c/jsmn/jsnm-internal
[jsmn-dump]: https://github.com/0x0916/notes/tree/master/code/c/jsmn/jsnm-dump
[jsmn-parser]: https://github.com/0x0916/notes/tree/master/code/c/jsmn/jsnm-parser


