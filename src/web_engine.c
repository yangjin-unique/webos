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
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include "web_engine.h"
#include "log.h"
#include "http.h"
#include <errno.h>

#define MAX(a, b)	((a) > (b) ? (a) : (b))

/* global params */
int g_max_sock_fd = 0;


void
web_engine_creat(web_engine_t *engine)
{
	init_conn_pool(&engine->conn_pool);
	engine->http_listen_fd = web_sock_open(engine->http_port);
	engine->https_listen_fd = web_sock_open(engine->https_port);
	engine->ssl_ctx = ssl_init(engine->key, engine->cert);
	engine->http_parser_handler = http_parser_handler;
	engine->http_parser_disconnect_handler = http_parser_disconnect_handler;
	//	engine->http_send_response = http_send_resp;
}


/* handle new connection coming events, includes http and https connections */
void
handle_connect_events(web_engine_t *engine, web_conn_type_t type)
{
	/* todo */
	int listfd;
	web_connection_t *conn;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen;

	memset(&cliaddr, 0, sizeof(cliaddr));
	clilen = sizeof(cliaddr);
	listfd = (type == WEB_HTTP) ? engine->http_listen_fd : engine->https_listen_fd;
	connfd = accept(listfd, (struct sockaddr *)&cliaddr, &clilen);
	if (connfd < 0)
	{
		web_log(WEB_LOG_ERROR, "accpet failed (cause %d)\n", strerror(errno));
		return;
	}
	
	printf("new connection fd=%d\n", connfd);
	if (type == WEB_HTTP)
		conn = alloc_http_connection(connfd, &cliaddr, clilen);
	else
		conn = alloc_https_connection(engine->ssl_ctx,  connfd, &cliaddr, clilen);
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

	if (!IS_CONN_SSL(conn))
	{
		printf("http read.......\n");
		nread = recv(conn->connfd, conn->rbuf, READ_BUF_SIZE, MSG_DONTWAIT);
	}
	else 
	{
		printf("https read .......\n");
		nread = SSL_read(conn->ssl, conn->rbuf, READ_BUF_SIZE); 
	}
	if (nread <= 0)
	{
		/* connection closed by client */
		/* ----------Todo: when to free connection----------- */
		//remove_conn_from_pool(&engine->conn_pool, conn);
		return;
	}
	conn->rbuf[nread] = 0;
	printf("****************recv %d bytes: %s****************\n", nread, conn->rbuf);
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


ssize_t
web_write(web_connection_t *conn, const void *buf, size_t size)
{
	int nwrite = 0;

	if (!IS_CONN_SSL(conn))
		nwrite = write(conn->connfd, buf, size);
	else
	{
		if (conn->ssl == NULL)
			return nwrite;
		nwrite = SSL_write(conn->ssl, buf, size);
	}
	return nwrite;
}


void
send_response(web_connection_t *conn)
{
	int nbytes;

	printf("..........send response now .........\n");
	if ((nbytes =web_write(conn, conn->wbuf, conn->wsize)) != conn->wsize)
	{
		web_log(WEB_LOG_ERROR, "send failed: only %d bytes sent\n", nbytes);
		return;
	}

	conn->wbuf[conn->wsize] = 0;
	//printf("Response:\n%s\n", conn->wbuf);
	if (conn->finfo != NULL && conn->finfo->fbuf != NULL)
	{
		//printf("send file now...");
		if ((nbytes = web_write(conn, conn->finfo->fbuf, 
						conn->finfo->size) != conn->finfo->size))
		{
			web_log(WEB_LOG_ERROR, "send file failed\n");
			free(conn->finfo->fbuf);
			return;
		}
	}
}



/* handle write evevts for active connection */
void
handle_write_events(web_engine_t *engine, web_connection_t *conn)
{
	/* todo */
	send_response(conn);
	//printf("handle write events ...\n");
	if (IS_CONN_CLOSE(conn))
	{
		printf("remove a connection\n");
		remove_conn_from_pool(&engine->conn_pool, conn);
	}
}


void 
process_events(web_engine_t *engine, int nready, fd_set *readfds, fd_set *writefds)
{
	web_connection_t *conn, *proc_conn;
	/* new connection */
	if (FD_ISSET(engine->http_listen_fd, readfds))
	{
		/* todo: */
		printf("handle new connections\n");
		handle_connect_events(engine, WEB_HTTP);
		nready--;
	}

	if (FD_ISSET(engine->https_listen_fd, readfds))
	{
		web_log(WEB_LOG_DEBUG, "handle new https connections");
		handle_connect_events(engine, WEB_HTTPS);
		nready--;
	}

	print_all_connections(&engine->conn_pool);

	conn = get_first_conn_from_pool(&engine->conn_pool);
	while (conn != NULL && nready != 0)
	{
		proc_conn = conn;
		conn = get_next_conn_from_pool(conn);

		if (proc_conn->connfd < 0 || proc_conn->connfd > FD_SETSIZE)
		{
			printf("----------------->wrong fd=%d\n", conn->connfd);
			continue;
		}
		if (FD_ISSET(proc_conn->connfd, readfds))
		{
			/* todo */
			handle_read_events(engine, proc_conn);
			nready--;
		}
		if (FD_ISSET(proc_conn->connfd, writefds))			
		{	/* todo */
			handle_write_events(engine, proc_conn);
			nready--;
		}
#if 0
		if (IS_CONN_CLOSE(conn))
		{
			printf("remove a connection\n");
			remove_conn_from_pool(&engine->conn_pool, conn);
		}
#endif
	}

}


int
select_engine(web_engine_t *engine, fd_set *readfds, fd_set *writefds)
{
	int nready = 0;
	web_connection_t *conn;

	g_max_sock_fd = MAX(engine->http_listen_fd, engine->https_listen_fd);
	FD_SET(engine->http_listen_fd, readfds);
	FD_SET(engine->https_listen_fd, readfds);
	conn = get_first_conn_from_pool(&engine->conn_pool);
	
	while (conn != NULL) 
	{
		/* Todo: redesign */
		FD_SET(conn->connfd, readfds);
		if (IS_CONN_WRITE(conn))
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
