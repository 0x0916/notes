#define _GNU_SOURCE
#include <stdio.h>
#include <setjmp.h>
#include <sched.h>
#include <signal.h>

#define	JUMP_PARENT	0x00
#define JUMP_CHILD	0xA0
#define	JUMP_INIT	0xA1

struct clone_t	{
	char stack[4096] __attribute__((aligned(16)));
	char stack_ptr[0];

	jmp_buf	*env;
	int jmpval;
};

static int child_func(void *arg) __attribute__ ((noinline));
static int child_func(void *arg) {
	struct clone_t	*ca = (struct clone_t*)arg;
	// 通过longjmp跳转到合适的位置，ca->jmpval就是setjmp的返回值
	longjmp(*ca->env, ca->jmpval);
}

static int clone_parent(jmp_buf *env, int jmpval) __attribute__ ((noinline));
static int clone_parent(jmp_buf *env, int jmpval) {
	struct clone_t	ca = {
		.env	= env,
		.jmpval = jmpval,
	};
	// CLONE_PARENT: 创建的子进程的父进程是调用者的父进程，新进程与创建它的进程成了`兄弟`而不是`父子`
	// SIGCHLD： flags的低字节包含了子进程死亡的时候发送给父进程的信号
	return clone(child_func, ca.stack_ptr, CLONE_PARENT | SIGCHLD, &ca);
}

int main(int argc, char **argv) {

	jmp_buf	env;
	// 保存stack context/environment到env中，该env变量后续会被函数longjmp作为参数使用
	// setjmp会直接返回0，如果从longjmp返回，返回值为longjmp的第二个参数的值。
	switch (setjmp(env)) {
	case JUMP_PARENT:
		printf("parent: pid = %d, ppid = %d\n", getpid(), getppid());
		clone_parent(&env, JUMP_CHILD);
		sleep(3);
		break;
	case JUMP_CHILD:
		printf("child: pid = %d, ppid = %d\n", getpid(), getppid());
		clone_parent(&env, JUMP_INIT);
		sleep(1);
		break;
	case JUMP_INIT:
		printf("init: pid = %d, ppid = %d\n", getpid(), getppid());
		break;
	default:
		break;
	}

}
