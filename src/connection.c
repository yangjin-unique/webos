/*
 * =====================================================================================
 *
 *       Filename:  connection.c
 *
 *    Description: connection layer under select engine. we put all active connections
 *				   in a poll.
 *
 *        Version:  1.0
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
#include <sys/time.h>
#include "log.h"
#include "slist.h"
#include <errno.h>
#include "connection.h"

#define LISTEN_QUEUE_SIZE	1024

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
		printf("%d:\tfd=%d\tsrcport=%d\n", i, conn->connfd, ntohs(conn->cliaddr.sin_port));
		i++;
		conn = conn->next;
	}
	printf("\n|------------------------end---------------------------|\n\n");
}


/* put a connection to a pool */
void
add_conn_to_pool(web_conn_pool_t *pool, web_connection_t *conn)
{
	if (conn == NULL || pool == NULL)
		return;

	slist_add_tail((slist_node_t **)&pool->conn_list_head, (slist_node_t *)conn);
	pool->size++;
	//print_all_connections(pool);
}


/* remove a connection from pool */
void
remove_conn_from_pool(web_conn_pool_t *pool, web_connection_t *conn)
{
	if (conn == NULL || pool == NULL)
		return;

	slist_remove((slist_node_t **)&pool->conn_list_head, (slist_node_t *)conn);
	close(conn->connfd);
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
	}
	pool->size--;
	if (conn != NULL)
		free(conn);
	//print_all_connections(pool);
}


