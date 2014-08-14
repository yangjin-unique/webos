/*
 * =====================================================================================
 *
 *       Filename:  web_engine.c
 *
 *    Description: select engine implementation 
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
#include "web_engine.h"
#include "log.h"
#include "http.h"
#include <errno.h>

/* global parms */
int g_max_sock_fd = 0;


void
web_engine_creat(web_engine_t *engine)
{
	engine->http_listen_fd = web_sock_open(engine->http_port);
	engine->http_parser_handler = http_parser_handler;
	engine->http_parser_disconnect_handler = http_parser_disconnect_handler;
}


/* handle new connection coming events */
void
handle_connect_events(web_engine_t *engine)
{
	/* todo */
	web_connection_t *conn;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen;

	memset(&cliaddr, 0, sizeof(cliaddr));
	clilen = sizeof(cliaddr);
	connfd = accept(engine->http_listen_fd, (struct sockaddr *)&cliaddr, &clilen);
	if (connfd < 0)
	{
		web_log(WEB_LOG_ERROR, "accpet failed (cause %d)\n", strerror(errno));
		return;
	}
	printf("new connection fd=%d\n", connfd);
	conn = alloc_new_connection(connfd, &cliaddr, clilen);
	if (conn == NULL)
		return;

	add_conn_to_pool(&engine->conn_pool, conn);
}


/* handle read events for active connection */
void
handle_read_events(web_engine_t *engine, web_connection_t *conn)
{
	/* todo */
	int nread;

	nread = recv(conn->connfd, conn->rbuf, READ_BUF_SIZE, MSG_DONTWAIT);

	if (nread <= 0)
	{
		/* connection closed by client */
		remove_conn_from_pool(&engine->conn_pool, conn);
		return;
	}
	conn->rbuf[nread] = 0;
	printf("recv %d bytes: %s\n", nread, conn->rbuf);
	conn->rsize = nread;
	conn->prbuf = conn->rbuf;
#if 0
	if (send(conn->connfd, conn->rbuf, nread, 0) != nread)
	{
		web_log(WEB_LOG_ERROR, "send failed\n");
		remove_conn_from_pool(&engine->conn_pool, conn);
	}
#endif
	engine->http_parser_handler(conn);
}


/* handle write evevts for active connection */
void
handle_write_events(web_connection_t *conn)
{
	/* todo */
	//printf("handle write events ...\n");
}


void 
process_events(web_engine_t *engine, int nready, fd_set *readfds, fd_set *writefds)
{
	web_connection_t *conn;
	/* new connection */
	if (FD_ISSET(engine->http_listen_fd, readfds))
	{
		/* todo: */
		printf("handle new connections\n");
		handle_connect_events(engine);
		nready--;
	}

	//print_all_connections(&engine->conn_pool);

	conn = get_first_conn_from_pool(&engine->conn_pool);
	while (conn != NULL && nready != 0)
	{
		if (FD_ISSET(conn->connfd, readfds))
		{
			/* todo */
			handle_read_events(engine, conn);
			nready--;
		}
		if (FD_ISSET(conn->connfd, writefds))			
		{	/* todo */
			handle_write_events(conn);
			nready--;
		}
		conn = get_next_conn_from_pool(conn);
	}

}



int
select_engine(web_engine_t *engine, fd_set *readfds, fd_set *writefds)
{
	int nready = 0;
	web_connection_t *conn;

	g_max_sock_fd = engine->http_listen_fd;
	FD_SET(engine->http_listen_fd, readfds);
	conn = get_first_conn_from_pool(&engine->conn_pool);
	
	while (conn != NULL) 
	{
		/* Todo: redesign */
		FD_SET(conn->connfd, readfds);

		FD_SET(conn->connfd, writefds);

		if(conn->connfd > g_max_sock_fd)
			g_max_sock_fd = conn->connfd;

		conn = get_next_conn_from_pool(conn);
	}

	nready = select(g_max_sock_fd + 1, readfds, writefds, NULL, NULL);

	if (nready < 0)
	{
		web_log(WEB_LOG_ERROR, "select failed\n");
	}

	return nready;
}


void 
web_engine_event_loop(web_engine_t *engine)
{
	int nready = 0;
	fd_set	readfds, writefds;
	//struct timeval timeout;
	g_max_sock_fd = engine->http_listen_fd;


	while (1)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		
		/* add read events */
		if ((nready = select_engine(engine, &readfds, &writefds)) > 0)
		{
			//printf("%d sockets ready ......\n", nready);
			process_events(engine, nready, &readfds, &writefds);
		}
	}
}
