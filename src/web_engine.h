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
#ifndef _WEB_ENGINE_H
#define _WEB_ENGINE_H

#include "connection.h"

typedef struct web_engine
{
	int		http_port;
	int		https_port;
	int		http_listen_fd;
	int		https_listen_fd;
	web_conn_pool_t		conn_pool; /* put all active conns in a list pool */
	void	(*http_parser_handler)(web_connection_t *conn);
	void	(*http_parser_disconnect_handler)(web_connection_t *conn);
	void	(*http_send_response)(web_connection_t *conn);
}web_engine_t;


void web_engine_creat(web_engine_t *engine);
void web_engine_event_loop(web_engine_t *engine);

#endif
