/*
 * =====================================================================================
 *
 *       Filename:  daemon.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include "log.h"

void
signal_handler(int sig)
{
	switch (sig)
	{
		case SIGHUP:
			//Todo: rehash server
			break;
		case SIGTERM:
			//Todo: shutdown server
			break;
		default:
			break;
	}
}


int
daemonize(char *lock_file)
{
	int fd, lfd;
	int i;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;
	char str[256] = {0};
	
	umask(027);

	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: get file limit failed\n");
		return -1;
	}
	if ((pid = fork()) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: fork failed\n");
		return -1;
	}
	else if (pid > 0) /* parent exit */
		return 0;

	setsid();

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGCHLD, &sa, NULL) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: sigaction failed\n");
		return -1;
	}

	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: sigaction failed\n");
		return -1;
	}
	if (sigaction(SIGTERM, &sa, NULL) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: sigaction failed\n");
		return -1;
	}

	if ((pid = fork()) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: fork failed\n");
		return -1;
	}
	else if (pid > 0) /* parent exit */
		return 0;

#if 0
	if (chdir("/") < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: chdir failed\n");
		exit(1);
	}
#endif
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	fd = open("/dev/null", O_RDWR);
	if (dup(fd) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: dup failed\n");
		return -1;
	}
	if (dup(fd) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: dup failed\n");
		return -1;
	}

	lfd = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0640);
	if (lfd < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: can not open %s\n", lock_file);
		return -1;
	}
	if (lockf(lfd, F_TLOCK, 0) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: can not lock %s\n", lock_file);
		return -1;
	}

	sprintf(str, "%d\n", getpid());
	if (write(lfd, str, strlen(str)) < 0)
	{
		web_log(WEB_LOG_ERROR, "daemon: write failed\n");
		return -1;
	}

	return 0;
}
