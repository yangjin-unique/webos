/*
 * =====================================================================================
 *
 *       Filename:  web_engine.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */
#ifndef _CORE_ENGINE_H
#define _CORE_ENGINE_H

#include "ssl.h"
#include "listen.h"

typedef struct _web_engine
{
    listen_opt_t http;
    listen_opt_t https;
	char	*key; /* ssl server key file */
	char	*cert; /* ssl server certificate file */
	SSL_CTX	*ssl_ctx; /* ssl context */
	//web_conn_pool_t		conn_pool; /* put all active conns in a list pool */
}web_engine_t;


typedef enum web_conn_type
{
	WEB_HTTP,
	WEB_HTTPS,
}web_conn_type_t;

void core_engine_init(web_engine_t *engine, int http_port, int https_port);
void core_engine_run(void);

#endif
