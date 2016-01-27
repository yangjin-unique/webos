/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

//TODO: user config
#define WEB_LOG_CURRENT_LEVEL	WEB_LOG_WARNNING

typedef enum {
	WEB_LOG_DEBUG,
	WEB_LOG_WARNNING,
	WEB_LOG_EVENT,
	WEB_LOG_ERROR,
}log_level_t;



int log_init(char *logfile);
void web_log(log_level_t level, const char *format, ...);


#endif
