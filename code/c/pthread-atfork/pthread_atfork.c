// gcc pthread_atfork.c -pthread
#define _GNU_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>



static pthread_mutex_t thread_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
//static pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

static void lock(void)
{
	int ret;
	if ((ret = pthread_mutex_lock(&thread_mutex)) != 0) {
		fprintf(stderr, "[%d]pthread_mutex_lock returned:%d %m\n", getpid(), ret);
	}
}

static void  unlock(void)
{
	int ret;
	if ((ret = pthread_mutex_unlock(&thread_mutex)) != 0) {
		fprintf(stderr, "[%d]pthread_mutex_unlock returned:%d %m\n", getpid(),  ret);
	}
}

__attribute__((constructor))
static void  setup_atfork(void)
{
	pthread_atfork(lock, unlock, unlock);
}

int main(int argc, char **argv) {
	pid_t pid;
	int wstatus;
	pid_t w;

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork error\n");
		exit(-1);
	}

	// parent
	if (pid != 0) {
		fprintf(stderr, "[%d]parent\n", getpid());
	}else {
		// child
		fprintf(stderr, "[%d]child\n", getpid());
		w = waitpid(pid, NULL, 0);
		fprintf(stderr, "[%d]child\n", getpid());
	}
}
