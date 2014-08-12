/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  server log module 
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#include "log.h"


FILE *g_logfile = NULL;

int 
log_init(char *logfile)
{
	assert(logfile != NULL);
	if ((g_logfile = fopen(logfile, "w")) == NULL)
	{
		return -1;
	}

	return 0;
}

void
web_log(log_level_t level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	if (level >= WEB_LOG_CURRENT_LEVEL)
	{
#if DAEMON
		vfprintf(g_logfile, format, args);
		fflush(g_logfile);	
#else
		vfprintf(stdout, format, args);
		fflush(stdout);
#endif

	}
	va_end(args);
}


