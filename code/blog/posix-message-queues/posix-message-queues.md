`POSIX message queues` allow processes to exchange data in the form of messages. 与`System V message queues`相比较，它有些需要注意的区别：

* POSIX message queues are reference counted.(A queue that is marked for deletion is removed only after it is closed by all process that are using it)
* POSIX message have an associated priority, and messages are always strictly queued(and thus received) in priority order.
* POSIX message queues provide a feature that allows a process to be asynchronously notified when an message is available on a queue.

## Overview

主要的`POSIX message queues API`包含如下：

* mq_open()
* mq_send()
* mq_receive()
* mq_close()
* mq_unlink()
* mq_getattr()
* mq_setattr()
* mq_notify()


