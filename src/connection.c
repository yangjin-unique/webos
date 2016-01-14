/*
 * =====================================================================================
 *
 *       Filename:  connection.c
 *
 *    Description: connection layer under select engine. we put all active connections
 *				   in a poll.
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "log.h"
#include "slist.h"
#include <errno.h>
#include "connection.h"

#define LISTEN_QUEUE_SIZE	1024

#if COMMENT

int 
web_sock_open(int port)
{
	int sock;
	struct sockaddr_in servaddr;
	int optval = 1;

	bzero(&servaddr, sizeof(servaddr));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		web_log(WEB_LOG_ERROR, "socket creating failed (cause: %s)\n", strerror(errno));
		exit(-1);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		web_log(WEB_LOG_ERROR, "set socket option failedi (cause: %s)\n", strerror(errno));
		exit(-1);
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		web_log(WEB_LOG_ERROR, "bind failed (cause: %s)\n", strerror(errno));
		exit(-1);
	}

	if (listen(sock, LISTEN_QUEUE_SIZE) < 0)
	{
		web_log(WEB_LOG_ERROR, "listen on socket failed (cause: %s)\n", strerror(errno));
		exit(-1);
	}

	return sock;
}


web_connection_t *
get_first_conn_from_pool(web_conn_pool_t *pool)
{
	return pool->conn_list_head;
}

web_connection_t *
get_next_conn_from_pool(web_connection_t *conn)
{
	return conn->next;
}


int
set_nonblock(int fd)
{
	int flag;
	
	flag = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)
	{
		web_log(WEB_LOG_ERROR, "connection fd set nonblock failed (cause: %s)\n", strerror(errno));
		return -1;
	}
	return 0;
}


/* set ssl connection params */ 
SSL *
get_ssl_ctx(SSL_CTX *ssl_ctx, int fd)
{
	SSL *cli_ctx;
	int ret;

	cli_ctx = SSL_new(ssl_ctx);
	if (cli_ctx == NULL)
	{
		web_log(WEB_LOG_ERROR, "creat client ssl context failed (SSL_new: %s)\n",
						ERR_reason_error_string(ERR_get_error()));
		return NULL;
	}
	if (SSL_set_fd(cli_ctx, fd) <= 0)
	{
		web_log(WEB_LOG_ERROR, "creat client ssl context failed (SSL_set_fd: %s)\n",
						ERR_reason_error_string(ERR_get_error()));
		SSL_free(cli_ctx);
		return NULL;
	}
	if ((ret = SSL_accept(cli_ctx)) <= 0)
	{
		web_log(WEB_LOG_ERROR, "creat client ssl context failed (SSL_accept: %d, %s, ret=%d)\n",
						ERR_get_error(), ERR_error_string(ERR_get_error(), NULL), ret);
		web_log(WEB_LOG_ERROR, "ssl_accpet error (cause: %s)\n", strerror(errno));
		SSL_free(cli_ctx);
		return NULL;
	}
	return cli_ctx;
}


/* alloc a new connection memory resource */
web_connection_t *
alloc_new_connection(int fd, struct sockaddr_in *cliaddr, socklen_t clilen)
{
	web_connection_t *conn;

	conn = (web_connection_t *)malloc(sizeof(web_connection_t));

	if (conn == NULL) 
	{
		web_log(WEB_LOG_ERROR, "alloc connection failed\n");
		goto alloc_conn_err;
	}
	memset(conn, 0, sizeof(web_connection_t));
	conn->connfd = fd;
	bcopy(cliaddr, &conn->cliaddr, clilen);
	/* alloc read buffer here */
	if ((conn->rbuf = (char *)malloc(READ_BUF_SIZE)) == NULL)
	{
		web_log(WEB_LOG_ERROR, "alloc connection rbuf failed\n");
		goto alloc_rbuf_err;
	}
	conn->prbuf = conn->rbuf;
	if ((conn->wbuf = (char *)malloc(WRITE_BUF_SIZE)) == NULL)
	{
		web_log(WEB_LOG_ERROR, "alloc connection wbuf failed\n");
		goto alloc_wbuf_err;
	}
	return conn;

alloc_wbuf_err:
	free(conn->rbuf);
	conn->rbuf = NULL;
alloc_rbuf_err:
	free(conn);
	conn = NULL;
alloc_conn_err:	
	return conn;
}


web_connection_t *
alloc_https_connection(SSL_CTX *ssl_ctx, int fd, struct sockaddr_in *cliaddr, socklen_t clilen)
{
	web_connection_t *conn;

	if ((conn = alloc_new_connection(fd, cliaddr, clilen)) == NULL)
	{
		close(fd);
		return NULL;
	}

	SET_CONN_SSL(conn); /* set ssl flag */
	conn->ssl = get_ssl_ctx(ssl_ctx, fd);
	if (conn->ssl == NULL)
	{
		close (fd);
		return NULL;
	}
	
	//set_nonblock(fd);	
	return conn;
}


web_connection_t *
alloc_http_connection(int fd, struct sockaddr_in *cliaddr, socklen_t clilen)
{
	web_connection_t *conn;

	set_nonblock(fd);
	if ((conn = alloc_new_connection(fd, cliaddr, clilen)) == NULL)
	{
		close(fd);
		return NULL;
	}
	return conn;	
}


/* print all connections, for debug use only */
void
print_all_connections(web_conn_pool_t *pool)
{
	printf("\n|-------------------connections pool-------------------|\n");
	printf("total connections: %d\n", pool->size);
	int i = 1;
	web_connection_t *conn;
	conn = pool->conn_list_head;
	while (conn != NULL) 
	{
		printf("%d:\tfd=%d\tsrcport=%d", i, conn->connfd, ntohs(conn->cliaddr.sin_port));
		printf("\t%s\n", IS_CONN_SSL(conn) ? "HTTPS" : "HTTP");
		i++;
		conn = conn->next;
	}
	printf("\n|------------------------end---------------------------|\n\n");
}


void
init_conn_pool(web_conn_pool_t *pool)
{
	if (pool == NULL)
		return;

	pool->size = 0;
	pool->conn_list_head = NULL;
}


/* put a connection to a pool */
void
add_conn_to_pool(web_conn_pool_t *pool, web_connection_t *conn)
{
	if (conn == NULL || pool == NULL)
		return;

	slist_add_tail((slist_node_t **)&pool->conn_list_head, (slist_node_t *)conn);
	pool->size++;
}


void
free_connection(web_connection_t *conn)
{
	if (conn->rbuf != NULL)
	{
		free(conn->rbuf);
		conn->rbuf = NULL;
	}
	if (conn->wbuf != NULL)
	{
		free(conn->wbuf);
		conn->wbuf = NULL;
	}
	if (conn->finfo != NULL)
	{
		if (conn->finfo->fbuf != NULL)
			free(conn->finfo->fbuf);
		free(conn->finfo);
		conn->finfo = NULL;
	}

	if (IS_CONN_SSL(conn))
	{
		SSL_shutdown(conn->ssl);
		SSL_free(conn->ssl);
		conn->ssl = NULL;
	}

	if (conn->cgi != NULL)
	{
		if (conn->cgi->env != NULL)
		{
			cgi_free_env_table(conn->cgi->env);
			conn->cgi->env = NULL;
		}
		free(conn->cgi);
		conn->cgi = NULL;
	}
	close(conn->connfd);
	free(conn);
}


/* remove a connection from pool */
void
remove_conn_from_pool(web_conn_pool_t *pool, web_connection_t *conn)
{
	if (conn == NULL || pool == NULL)
		return;

	slist_remove((slist_node_t **)&pool->conn_list_head, (slist_node_t *)conn);
	free_connection(conn);
	pool->size--;
	printf("pool size=%d\n", pool->size);
	//print_all_connections(pool);
}

#endif


void
connection_write_handler(event_data_t *ev_data)
{
    //connection_t *conn = (connection_t *)ev_data->data;

    
}


static void
connection_read_handler(event_data_t *ev_data)
{
    connection_t *conn = (connection_t *) ev_data->data;
    ssize_t n;

    printf("connection read handler: fd=%d\n", conn->fd);
    n = read(conn->fd, conn->rbuf, 2048);
    if (n <= 0) {
        printf("read failed: n = %d\n", n);
        return;
    }
    conn->rbuf[n] = 0;
    printf("read conn fd: %d\n", conn->fd);
    printf("read content: %s\n", conn->rbuf);
}

static connection_t *
connection_alloc(void)
{
    connection_t *conn;

    conn = (connection_t *) malloc(sizeof(connection_t));
    if (conn == NULL) {
        web_log(WEB_LOG_ERROR, "connection alloc failed\n");
        return NULL;
    }
    memset(conn, 0, sizeof(connection_t));
    return conn;
}

void
connection_add(int fd)
{
    connection_t *conn = connection_alloc();

    printf("connection add\n");
    assert(conn);
    conn->fd = fd;
    conn->r_ev_data.data = conn;
    conn->r_ev_data.fd = fd;
    conn->r_ev_data.ev_handler = connection_read_handler;
    event_add(&conn->r_ev_data, EVENT_TYPE_READ);
}
