/*
 * =====================================================================================
 *
 *       Filename:  connection.h
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
#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <netinet/in.h>
/* per connection structure */
typedef struct web_connection
{
	struct web_connection	*next;
	int						connfd;
	struct sockaddr_in		cliaddr;
#define READ_BUF_SIZE	8*1024
#define WRITE_BUF_SIZE	8*1024
	char					*rbuf; /* read buffer */
	char					*wbuf; /* write buffer */
	char					*prbuf; /* currently parsed position in rbuf, used for http parser */
	int						rsize; /* read buffer size */
	int						wsize; /* write buffer size */
	/* http parsed params */
	int						status;
	int						method; /* http request method: GET, HEAD, POST */
	char					*host;
	char					*proto; /* HTTP protocol version (HTTP/1.1) */
	char					*uri;
}web_connection_t;

/* connection pool, hold all active conntions */
typedef struct web_conn_pool
{
	int		size;
	web_connection_t	*conn_list_head;
}web_conn_pool_t;


int web_sock_open(int port);
web_connection_t *get_first_conn_from_pool(web_conn_pool_t *pool);
web_connection_t *get_next_conn_from_pool(web_connection_t *conn);
web_connection_t *alloc_new_connection(int fd, struct sockaddr_in *cliaddr, socklen_t clilen);
void print_all_connections(web_conn_pool_t *pool);
void add_conn_to_pool(web_conn_pool_t *pool, web_connection_t *conn);
void remove_conn_from_pool(web_conn_pool_t *pool, web_connection_t *conn);

#endif
