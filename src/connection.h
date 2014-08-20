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


/* connection flag bit use */
#define CONN_FLAG_CLOSE			0x0001 /* connection to be closed */
#define	CONN_FLAG_WRITE			0x0002 /* conn has data to write */

#define SET_CONN_CLOSE(conn)	(((conn)->flag) |= CONN_FLAG_CLOSE)
#define IS_CONN_CLOSE(conn)		!!(((conn)->flag) & CONN_FLAG_CLOSE)
#define SET_CONN_WRITE(conn)	(((conn)->flag) |= CONN_FLAG_WRITE)
#define IS_CONN_WRITE(conn)		!!(((conn)->flag) & CONN_FLAG_WRITE)

typedef struct file_info
{
	int		fd;
	int		size;
	time_t	mtime; /* last modified time */
	int		type; /* file type: html, css, jpeg, png, gif, ... */
	char	*fbuf;
}file_info_t;

/* per connection structure */
typedef struct web_connection
{
	struct web_connection	*next;
	int						connfd;
	struct sockaddr_in		cliaddr;
#define READ_BUF_SIZE	8*1024
#define WRITE_BUF_SIZE	1*1024
	char					*rbuf; /* read buffer */
	char					*wbuf; /* write buffer */
	char					*prbuf; /* currently parsed position in rbuf, used for http parser */
	int						rsize; /* read buffer size */
	int						wsize; /* write buffer size */
	/* http state machine */
	int						status;
	/* http parsed params */
	int						method; /* http request method: GET, HEAD, POST */
	char					*host;
	char					*proto; /* HTTP protocol version (HTTP/1.1) */
	char					*uri;
	char					*user_agent;
	int						cont_type; /* content type */
	int						cont_len; /* content length */
	int						conn_type; /* connection type: keep-alive, close */
	file_info_t				*finfo;
	unsigned int			flag;
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
