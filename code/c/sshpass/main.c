/*  This file is part of "sshpass", a tool for batch running password ssh authentication
 *  Copyright (C) 2006, 2015 Lingnu Open Source Consulting Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version, provided that it was accepted by
 *  Lingnu Open Source Consulting Ltd. as an acceptable license for its
 *  projects. Consult http://www.lingnu.com/licenses.html
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// 注释掉`config.h`
/*
#if HAVE_CONFIG_H
#include "config.h"
#endif
*/
// 添加必须的宏定义

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.06"
/* Password prompt to use */
#define PASSWORD_PROMPT "assword"
/* Define to the full name of this package. */
#define PACKAGE_NAME "sshpass"
/* Define to the full name and version of this package. */
#define PACKAGE_STRING "sshpass 1.06"
/* Define to 1 if you have the `posix_openpt' function. */
#define HAVE_POSIX_OPENPT 1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#if HAVE_TERMIOS_H
#include <termios.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// 定义程序的返回错误编码
enum program_return_codes {
    RETURN_NOERROR,
    RETURN_INVALID_ARGUMENTS,
    RETURN_CONFLICTING_ARGUMENTS,
    RETURN_RUNTIME_ERROR,
    RETURN_PARSE_ERRROR,
    RETURN_INCORRECT_PASSWORD,
    RETURN_HOST_KEY_UNKNOWN,
    RETURN_HOST_KEY_CHANGED,
};

// 一些系统没有定义该函数，所以程序自己在这里实现了。
#ifndef HAVE_POSIX_OPENPT
int
posix_openpt(int flags)
{
    return open("/dev/ptmx", flags);
}
#endif

int runprogram( int argc, char *argv[] );

//  全局数据结构，保存程序的配置信息
struct {
    enum { PWT_STDIN, PWT_FILE, PWT_FD, PWT_PASS } pwtype;
    union {
	const char *filename;
	int fd;
	const char *password;
    } pwsrc;

    const char *pwprompt;
    int verbose;
} args;

static void show_help()
{
    printf("Usage: " PACKAGE_NAME " [-f|-d|-p|-e] [-hV] command parameters\n"
	    "   -f filename   Take password to use from file\n"
	    "   -d number     Use number as file descriptor for getting password\n"
	    "   -p password   Provide password as argument (security unwise)\n"
	    "   -e            Password is passed as env-var \"SSHPASS\"\n"
	    "   With no parameters - password will be taken from stdin\n\n"
            "   -P prompt     Which string should sshpass search for to detect a password prompt\n"
            "   -v            Be verbose about what you're doing\n"
	    "   -h            Show help (this screen)\n"
	    "   -V            Print version information\n"
	    "At most one of -f, -d, -p or -e should be used\n");
}

// 解析命令行参数，将结果填充到`args`这个全局变量中.成功时，返回`argv offset`,
// 失败时，返回一个负数。
static int parse_options( int argc, char *argv[] )
{
    int error=-1;
    int opt;

    // 默认密码从标准输入中获取
    args.pwtype=PWT_STDIN;
    args.pwsrc.fd=0;

#define VIRGIN_PWTYPE if( args.pwtype!=PWT_STDIN ) { \
    fprintf(stderr, "Conflicting password source\n"); \
    error=RETURN_CONFLICTING_ARGUMENTS; }

    while( (opt=getopt(argc, argv, "+f:d:p:P:heVv"))!=-1 && error==-1 ) {
	switch( opt ) {
	case 'f':
	    // 从文件中获取密码
	    VIRGIN_PWTYPE;

	    args.pwtype=PWT_FILE;
	    args.pwsrc.filename=optarg;
	    break;
	case 'd':
	    // 从文件描述符中获取密码
	    VIRGIN_PWTYPE;

	    args.pwtype=PWT_FD;
	    args.pwsrc.fd=atoi(optarg);
	    break;
	case 'p':
	    // 从命令行中获取密码
	    VIRGIN_PWTYPE;

	    args.pwtype=PWT_PASS;
	    args.pwsrc.password=strdup(optarg);

            // 为了安全起见，隐藏命令行中的密码
            {
                int i;

                for( i=0; optarg[i]!='\0'; ++i )
                    optarg[i]='z';
            }
	    break;
        case 'P':
            args.pwprompt=optarg;
            break;
        case 'v':
            args.verbose++;
            break;
	case 'e':
	    // 从环境变量`SSHPASS`中获取密码
	    VIRGIN_PWTYPE;

	    args.pwtype=PWT_PASS;
	    args.pwsrc.password=getenv("SSHPASS");
            if( args.pwsrc.password==NULL ) {
                fprintf(stderr, "sshpass: -e option given but SSHPASS environment variable not set\n");

                error=RETURN_INVALID_ARGUMENTS;
            }
	    break;
	case '?':
	case ':':
	    error=RETURN_INVALID_ARGUMENTS;
	    break;
	case 'h':
	    error=RETURN_NOERROR;
	    break;
	case 'V':
	    printf("%s\n"
                    "(C) 2006-2011 Lingnu Open Source Consulting Ltd.\n"
                    "(C) 2015-2016 Shachar Shemesh\n"
		    "This program is free software, and can be distributed under the terms of the GPL\n"
		    "See the COPYING file for more information.\n"
                    "\n"
                    "Using \"%s\" as the default password prompt indicator.\n", PACKAGE_STRING, PASSWORD_PROMPT );
	    exit(0);
	    break;
	}
    }

    if( error>=0 )
	return -(error+1);
    else
	return optind;
}

int main( int argc, char *argv[] )
{

    // 解析命令行参数，填充全局变量`args`
    int opt_offset=parse_options( argc, argv );

    if( opt_offset<0 ) {
	// There was some error
	show_help();

        return -(opt_offset+1); // -1 becomes 0, -2 becomes 1 etc.
    }

    if( argc-opt_offset<1 ) {
	show_help();

        return 0;
    }

	// 执行ssh命令
    return runprogram( argc-opt_offset, argv+opt_offset );
}

int handleoutput( int fd );

/* Global variables so that this information be shared with the signal handler */
static int ourtty; // Our own tty
static int masterpt;

void window_resize_handler(int signum);
void sigchld_handler(int signum);

int runprogram( int argc, char *argv[] )
{
    struct winsize ttysize; // The size of our tty

    // We need to interrupt a select with a SIGCHLD. In order to do so, we need a SIGCHLD handler
    signal( SIGCHLD,sigchld_handler );

    // 为我们的进程创建 `pseudo terminal`
    masterpt=posix_openpt(O_RDWR);

    if( masterpt==-1 ) {
	perror("Failed to get a pseudo terminal");

	return RETURN_RUNTIME_ERROR;
    }

    // 设置`masterpt`的status flag `O_NONBLOCK`
    fcntl(masterpt, F_SETFL, O_NONBLOCK);

	// 设置为终端的权限，允许访问slave端
    if( grantpt( masterpt )!=0 ) {
	perror("Failed to change pseudo terminal's permission");

	return RETURN_RUNTIME_ERROR;
    }
	//  unlock a pseudoterminal master/slave pair
    if( unlockpt( masterpt )!=0 ) {
	perror("Failed to unlock pseudo terminal");

	return RETURN_RUNTIME_ERROR;
    }

// 打开程序的控制终端
    ourtty=open("/dev/tty", 0);
// 设置masterpt终端的大小为ourtty终端的大小
    if( ourtty!=-1 && ioctl( ourtty, TIOCGWINSZ, &ttysize )==0 ) {
	// 注册信号处理函数 `window_resize_handler`
        signal(SIGWINCH, window_resize_handler);

        ioctl( masterpt, TIOCSWINSZ, &ttysize );
    }
// 或得伪终端 slave端的名称
    const char *name=ptsname(masterpt);
    int slavept;
    /*
       Comment no. 3.14159

       This comment documents the history of code.

       We need to open the slavept inside the child process, after "setsid", so that it becomes the controlling
       TTY for the process. We do not, otherwise, need the file descriptor open. The original approach was to
       close the fd immediately after, as it is no longer needed.

       It turns out that (at least) the Linux kernel considers a master ptty fd that has no open slave fds
       to be unused, and causes "select" to return with "error on fd". The subsequent read would fail, causing us
       to go into an infinite loop. This is a bug in the kernel, as the fact that a master ptty fd has no slaves
       is not a permenant problem. As long as processes exist that have the slave end as their controlling TTYs,
       new slave fds can be created by opening /dev/tty, which is exactly what ssh is, in fact, doing.

       Our attempt at solving this problem, then, was to have the child process not close its end of the slave
       ptty fd. We do, essentially, leak this fd, but this was a small price to pay. This worked great up until
       openssh version 5.6.

       Openssh version 5.6 looks at all of its open file descriptors, and closes any that it does not know what
       they are for. While entirely within its prerogative, this breaks our fix, causing sshpass to either
       hang, or do the infinite loop again.

       Our solution is to keep the slave end open in both parent AND child, at least until the handshake is
       complete, at which point we no longer need to monitor the TTY anyways.
     */

    int childpid=fork();
    if( childpid==0 ) {
	// 子进程

	// 新建一个会话，并设置进程组ID
	setsid();

	// 让伪终端slave端成为我们的控制终端,我们不需要一直打开它，所以关闭掉
        slavept=open(name, O_RDWR );
        close( slavept );

	// 关闭 伪终端的master端，因为在子进程中不需要它
	close( masterpt );

	char **new_argv=malloc(sizeof(char *)*(argc+1));

	int i;

	for( i=0; i<argc; ++i ) {
	    new_argv[i]=argv[i];
	}

	new_argv[i]=NULL;
	// 在子进程中执行ssh程序
	execvp( new_argv[0], new_argv );

	perror("sshpass: Failed to run command");

	exit(RETURN_RUNTIME_ERROR);
    } else if( childpid<0 ) {
	perror("sshpass: Failed to create child process");

	return RETURN_RUNTIME_ERROR;
    }

    // 父进程
    slavept=open(name, O_RDWR|O_NOCTTY );

    int status=0;
    int terminate=0;
    pid_t wait_id;
    sigset_t sigmask, sigmask_select;

    // Set the signal mask during the select
    sigemptyset(&sigmask_select);

    // And during the regular run
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGCHLD);

    sigprocmask( SIG_SETMASK, &sigmask, NULL );

    do {
	if( !terminate ) {
	    fd_set readfd;

	    FD_ZERO(&readfd);
	    FD_SET(masterpt, &readfd);
		// 监听masterpt
	    int selret=pselect( masterpt+1, &readfd, NULL, NULL, NULL, &sigmask_select );

	    if( selret>0 ) {
		if( FD_ISSET( masterpt, &readfd ) ) {
                    int ret;
		    if( (ret=handleoutput( masterpt )) ) {
			// ssh 登录出错

                        // handleoutput 出错时，返回一个正数的错误码
                        if( ret>0 ) {
                            close( masterpt ); // Signal ssh that it's controlling TTY is now closed
                            close(slavept);
                        }

			terminate=ret;

                        if( terminate ) {
                            close( slavept );
                        }
		    }
		}
	    }
		// 判断子进程是否退出，并立即返回
	    wait_id=waitpid( childpid, &status, WNOHANG );
	} else {
		// 等待ssh进程退出
	    wait_id=waitpid( childpid, &status, 0 );
	}
    } while( wait_id==0 || (!WIFEXITED( status ) && !WIFSIGNALED( status )) );

    if( terminate>0 )
	return terminate;
    else if( WIFEXITED( status ) )
	return WEXITSTATUS(status);
    else
	return 255;
}

int match( const char *reference, const char *buffer, ssize_t bufsize, int state );
void write_pass( int fd );

int handleoutput( int fd )
{
    // We are looking for the string
    static int prevmatch=0; // If the "password" prompt is repeated, we have the wrong password.
    static int state1, state2;
    static int firsttime = 1;
    static const char *compare1=PASSWORD_PROMPT; // Asking for a password
    static const char compare2[]="The authenticity of host "; // Asks to authenticate host
    // static const char compare3[]="WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!"; // Warns about man in the middle attack
    // The remote identification changed error is sent to stderr, not the tty, so we do not handle it.
    // This is not a problem, as ssh exists immediately in such a case
    char buffer[256];
    int ret=0;

    if( args.pwprompt ) {
        compare1 = args.pwprompt;
    }

    if( args.verbose && firsttime ) {
        firsttime=0;
        fprintf(stderr, "SSHPASS searching for password prompt using match \"%s\"\n", compare1);
    }

    int numread=read(fd, buffer, sizeof(buffer)-1 );
    buffer[numread] = '\0';
    if( args.verbose ) {
        fprintf(stderr, "SSHPASS read: %s\n", buffer);
    }

    state1=match( compare1, buffer, numread, state1 );

    // Are we at a password prompt?
    if( compare1[state1]=='\0' ) {
	if( !prevmatch ) {
            if( args.verbose )
                fprintf(stderr, "SSHPASS detected prompt. Sending password.\n");
	    write_pass( fd );
	    state1=0;
	    prevmatch=1;
	} else {
	    // Wrong password - terminate with proper error code
            if( args.verbose )
                fprintf(stderr, "SSHPASS detected prompt, again. Wrong password. Terminating.\n");
	    ret=RETURN_INCORRECT_PASSWORD;
	}
    }

    if( ret==0 ) {
        state2=match( compare2, buffer, numread, state2 );

        // Are we being prompted to authenticate the host?
        if( compare2[state2]=='\0' ) {
            if( args.verbose )
                fprintf(stderr, "SSHPASS detected host authentication prompt. Exiting.\n");
            ret=RETURN_HOST_KEY_UNKNOWN;
        }
    }

    return ret;
}

// 如果匹配成功，返回reference的最后一个位置
int match( const char *reference, const char *buffer, ssize_t bufsize, int state )
{
    // This is a highly simplisic implementation. It's good enough for matching "Password: ", though.
    int i;
    for( i=0;reference[state]!='\0' && i<bufsize; ++i ) {
	if( reference[state]==buffer[i] )
	    state++;
	else {
	    state=0;
	    if( reference[state]==buffer[i] )
		state++;
	}
    }

    return state;
}

void write_pass_fd( int srcfd, int dstfd );

void write_pass( int fd )
{
    switch( args.pwtype ) {
    case PWT_STDIN:
	write_pass_fd( STDIN_FILENO, fd );
	break;
    case PWT_FD:
	write_pass_fd( args.pwsrc.fd, fd );
	break;
    case PWT_FILE:
	{
	    int srcfd=open( args.pwsrc.filename, O_RDONLY );
	    if( srcfd!=-1 ) {
		write_pass_fd( srcfd, fd );
		close( srcfd );
	    }
	}
	break;
    case PWT_PASS:
	write( fd, args.pwsrc.password, strlen( args.pwsrc.password ) );
	write( fd, "\n", 1 );
	break;
    }
}

//  读取srcfd中的密码，将其写入dstfd中
void write_pass_fd( int srcfd, int dstfd )
{

    int done=0;

    while( !done ) {
	char buffer[40];
	int i;
	int numread=read( srcfd, buffer, sizeof(buffer) );
	done=(numread<1);
	for( i=0; i<numread && !done; ++i ) {
	    if( buffer[i]!='\n' )
		write( dstfd, buffer+i, 1 );
	    else
		done=1;
	}
    }

    write( dstfd, "\n", 1 );
}

// 信号处理函数，当ourtty的终端大小变化时，同步修改masterpt的终端大小
void window_resize_handler(int signum)
{
    struct winsize ttysize; // The size of our tty

    if( ioctl( ourtty, TIOCGWINSZ, &ttysize )==0 )
        ioctl( masterpt, TIOCSWINSZ, &ttysize );
}

// Do nothing handler - makes sure the select will terminate if the signal arrives, though.
void sigchld_handler(int signum)
{
}