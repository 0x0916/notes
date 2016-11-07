#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

static void set_noecho(int fd);	//turn off echo on slave pty
void	do_driver(char *);	//changes our stdin/stdout
void	loop(int, int);		//copy stdin->ptym, ptym->stdout
int ptym_open(char *pts_name, int pts_namesz) {
	char *ptr;
	int fdm;

	// Return the name of the master device so that on failure
	// the caller can print an error message.
	strncpy(pts_name, "/dev/ptmx", pts_namesz);
	// Null terminate to handle case where string length > pts_namesz
	pts_name[pts_namesz - 1] = '\0';

	fdm = open("/dev/ptmx", O_RWDR);
	if (fdm < 0)
		return -1;
	// Grant access to slave
	if (grantpt(fdm) < 0) {
		close(fdm);
		return -2;
	}
	// clear slave's lock flag
	if (unlockpt(fdm) < 0) {
		close(fdm);
		return -3;
	}
	// get slave's name
	if ((ptr = ptsname(fdm)) == NULL) {
		close(fdm);
		return -4;
	}

	// Return name of slave, NULL terminate to handle case where strlen(pts_name) > pts_namesz
	strncpy(pts_name, ptr, pts_namesz);
	pts_name[pts_namesz - 1] = '\0';

	// return the fd of master
	return fdm;
}

int ptys_open(char *pts_name) {
	int fds;

	if ((fds = open(pts_name, O_RDWR)) < 0)
		return -5;

	return fds;
}

pid_t pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,
	const struct termios *slave_termios,
	const struct winsize *slave_winsize)
{
	int fdm, fds;
	pid_t	pid;
	char pts_name[20];

	if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0) {
		printf("can not open master pty: %s, error %d\n", pts_name, fdm);
		return -1;
	}

	if (slave_name != NULL) {
		strncpy(slave_name, pts_name, slave_namesz);
		slave_name[slave_namesz - 1] = '\0';
	}

	if ((pid = fork()) < 0){
		return -1;
	} else if (pid == 0) {	// child
		if (setsid() < 0) {
			printf("setsid error\n");
			exit(1);
		}
		if ((fds = ptys_open(pts_name)) < 0) {
			printf("can not open slave pty\n");
			exit(1);
		}
		// all done with master in child
		close(fdm);

		if (ioctl(fds, TIOCSCTTY, (char *)0) < 0 ) {
			printf("TIOCSCTTY error\n");
			exit(1);
		}

		// set slave's termios and window size
		if (slave_termios != NULL) {
			if (tcsetattr(fds, TCSANOW, slave_termios) < 0) {
				printf("tcsetattr error on slave pty");
				exit(1);
			}
		}
		if (slave_winsize != NULL) {
			if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0) {
				printf("TIOCSWINSZ error on slave pty");
				exit(1);
			}
		}

		// slave becomes stdin/stdout/stderr of child
		if (dup2(fds, STDIN_FILENO) != STDIN_FILENO) {
			printf("dup2 error to stdin\n");
			exit(1);
		}
		if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO) {
			printf("dup2 error to stdout\n");
			exit(1);
		}
		if (dup2(fds, STDERR_FILENO) != STDERR_FILENO) {
			printf("dup2 error to stderr\n");
			exit(1);
		}

		if (fds != STDIN_FILENO && fds != STDOUT_FILENO && fds != STDERR_FILENO) {
			close(fds);
		}
		// child return 0 just like fork()
		return 0;
	} else { // parent
		// return the fd of master
		*ptrfdm = fdm;
		// parent returns pid of child
		return pid;
	}

}

ssize_t             /* Write "n" bytes to a descriptor  */
writen(int fd, const void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;

	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return(-1); /* error, return -1 */
			else
				break;      /* error, return amount written so far */
		} else if (nwritten == 0) {
			break;
		}
		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n - nleft);      /* return >= 0 */
}

int tty_raw(int fd)		/* put terminal into a raw mode */
{
	int				err;
	struct termios	buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return(-1);
	save_termios = buf;	/* structure copy */

	/*
	 * Echo off, canonical mode off, extended input
	 * processing off, signal chars off.
	 */
	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	/*
	 * No SIGINT on BREAK, CR-to-NL off, input parity
	 * check off, don't strip 8th bit on input, output
	 * flow control off.
	 */
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	/*
	 * Clear size bits, parity checking off.
	 */
	buf.c_cflag &= ~(CSIZE | PARENB);

	/*
	 * Set 8 bits/char.
	 */
	buf.c_cflag |= CS8;

	/*
	 * Output processing off.
	 */
	buf.c_oflag &= ~(OPOST);

	/*
	 * Case B: 1 byte at a time, no timer.
	 */
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);

	/*
	 * Verify that the changes stuck.  tcsetattr can return 0 on
	 * partial success.
	 */
	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) ||
	  (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) ||
	  (buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8 ||
	  (buf.c_oflag & OPOST) || buf.c_cc[VMIN] != 1 ||
	  buf.c_cc[VTIME] != 0) {
		/*
		 * Only some of the changes were made.  Restore the
		 * original settings.
		 */
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}

	ttystate = RAW;
	ttysavefd = fd;
	return(0);
}

int main(int argc, char **argv) {
	int	fdm,c, ignoreeof, interactive, noecho, verbose;
	pid_t	pid;
	char	*driver;
	char	slave_name[20];
	struct	termios	orig_termios;
	struct	winsize	size;

	while ((c = getopt(argc, argv, "-d:einv")) != EOF) {
		switch(c) {
		case 'd': // driver for stdin/stdout
			driver = optarg;
			break;
		case 'e': // noecho for slave pty's line discipline
			noecho = 1;
			break;
		case 'i': // ignore EOF on standard input
			ignoreeof = 1;
			break;
		case 'n': // not interactive
			interactive = 0;
			break;
		case 'v': // verbose
			verbose = 1;
			break;
		case '?':
			printf("unrecognized option: -%c", optopt);
			exit(1);
		}
	}

	if (optind >= argc) {
		printf("usage: pty [-d driver -einv] program [arg ...]\n");
		exit(1);
	}

	// fetch current termios and window size
	if (interactive) {
		if (tcgetattr(STDIN_FILENO, &orig_termios) <0) {
			printf("tcgetattr error on stdin");
			exit(1);
		}

		if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *)&size) < 0) {
			printf("TIOCGWINSZ error");
			exit(1);
		}

		pid = pty_fork(&fdm, slave_name, sizeof(slave_name), &orig_termios, &size);
	}else {
		pid = pty_fork(&fdm, slave_name, sizeof(slave_name), NULL, NULL);
	}

	if (pid <0 ) {
		printf("fork error");
		exit(1);
	}else if(pid == 0) { // child
		if (noecho)
			// stdin is slave pty
			set_noecho(STDIN_FILENO);
		if (execvp(argv[optind], &argv[optind]) < 0) {
			printf("can not execute: %s\n", argv[optind]);
			exit(1);
		}
	}

	if (verbose) {
		fprintf(stderr, "slave name = %s\n", slave_name);
		if (driver != NULL)
			fprintf(stderr, "driver = %s\n", driver);
	}

	if (interactive && driver == NULL) {
		// user's tty to raw mode
		if (tty_raw(STDIN_FILENO) < 0) 
		{
			printf("tty_raw error\n");
			exit(1);
		}
		// reset user's tty on exit
		if (atexit(tty_atexit) < 0) {
			printf("atexit error\n");
			exit(1);
		}
	}

	// changes our stdin/stdout
	if (driver)
		do_driver(driver);

	// copyies stdin ->ptym, ptym -> stdout
	loop(fdm, ignoreeof);

	exit(0);
}


// turn off echo for slave pty
static void set_noecho(int fd) {
	struct termios	stermios;

	if (tcgetattr(fd, &stermios) < 0) {
		printf("tcgetattr error");
		exit(1);
	}

	stermios.c_lflags &= ~(ECHO| ECHOE | ECHOK | ECHONL);
	// also turnoff NL to CR/NL mapping on output
	stermios.c_oflag &= ~(ONLCR);

	if (tcsetattr(fd,TCSANOW, &stermios) < 0) {
		printf("tcsetattr error");
		exit(1);
	}

}

//changes our stdin/stdout
void	do_driver(char *driver) {
	pid_t	child;
	int	pipefd[2];

	// Create a stream pipe to communicate with the driver
	if (pipe(pipefd) <0) {
		printf("can not create stream pipe\n");
		exit(1);
	}

	if ((child = fork()) < 0) {
		printf("fork error\n");
		exit(1);
	}else if (child == 0) {// child
		close(pipefd[1]);

		// stdin for driver
		if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
			printf("dup2 error to stdin\n");
			exit(1);
		}
		// stdout for driver
		if (dup2(pipefd[0], STDOUT_FILENO) != STDOUT_FILENO) {
			printf("dup2 error to stdout\n");
			exit(1);
		}

		if (pipefd[0] != STDIN_FILENO && pipefd[0] != STDOUT_FILENO) {
			close(pipefd[0]);
		}

		// leave stderr for driver alone
		execlp(driver, driver, (char *)0);
		printf("execlp error for: %s\n", driver);
		exit(1);
	}

	close(pipefd[0]);		// parent
	if (dup2(pipefd[1], STDIN_FILENO) != STDIN_FILENO) {
		printf("dup2 error to stdin\n");
		exit(1);
	}
	if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) {
		printf("dup2 error to stdout\n");
		exit(1);
	}
	if (pipefd[1] != STDIN_FILENO && pipefd[1] != STDOUT_FILENO) {
		close(pipefd[1]);
	}

	// parent returns, but with stdin and stdout conected to the driver
}

static volatile sig_atomic_t	sigcaught;		// set by signal handler
static void sig_term(int signo) {
	sigcaught = 1;	// just set flag and return
}

#define	BUFFSIZE	512
//copy stdin->ptym, ptym->stdout
void	loop(int ptym, int ignoreeof) {
	pid_t	child;
	int	nread;
	char	buf[BUFFSIZE];

	if ((child = fork()) < 0) {
		printf("fork error\n");
		exit(1);
	}else if (child == 0){ // child copies stdin to ptym
		for (;;) {
			if ((nread = read(STDIN_FILENO, buf, BUFFSIZE)) < 0) {
				printf("read error form stdin\n");
				exit(1);
			}else if (nread == 0)
				break;
			if (writen(ptym, buf, nread) != nread) {
				printf("writen error form stdin\n");
				exit(1);
			}
		}
		// we always terminate when we encounter an EOF on stdin.
		// but we notify the parent only if ignoreeof is 0.
		if (ignoreeof == 0)
			kill(getppid(), SIGTERM);	// notify parent
		exit(0);

	}

	// Parent copies ptym to stdout
	struct sigaction	act, oact;
	act.sa_handler = sig_term;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGTERM, &act, &oact) < 0) {
		printf("sigaction error for SIGTERM\n");
		exit(1);
	}

	for(;;) {
		if ((nread = read(ptym, buf, BUFFSIZE)) <= 0) {
			break;	// signal caught, error, or EOF
		}
		if (writen(STDOUT_FILENO, buf, nread) != nread) {
			printf("writen error to stdout\n");
			exit(1);
		}
	}

	if (sigcaught == 0)
		kill(child, SIGTERM);

	// Parent returns to caller 
}
