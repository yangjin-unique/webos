/*
 * =====================================================================================
 *
 *       Filename:  os.c
 *
 *    Description:  
 *
 *        Version:  2.0
 *        Created:  10/19/2015 07:21:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include "os.h"
#include "log.h"

#define LISTEN_QUEUE_SIZE   20

int 
os_open_listen_sock(int port)
{
	int sock;
	struct sockaddr_in servaddr;
	int optval = 1;

	bzero(&servaddr, sizeof(servaddr));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
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
		web_log(WEB_LOG_ERROR, "bind port(%d) failed (cause: %s)\n", port, strerror(errno));
		exit(-1);
	}

	if (listen(sock, LISTEN_QUEUE_SIZE) < 0)
	{
		web_log(WEB_LOG_ERROR, "listen on socket failed (cause: %s)\n", strerror(errno));
		exit(-1);
	}

	return sock;
}



int
os_set_nonblock(int fd)
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


/*********** os buffer APIs *************/
os_buf_t*
os_buf_malloc(int len)
{
    os_buf_t *buf;

    buf = os_calloc(1, sizeof(os_buf_t));
    if (!buf) {
        web_log(WEB_LOG_ERROR, "os_buf: buffer malloc error - %s", strerror(errno));
        return NULL;
    }
    buf->data = os_calloc(len, sizeof(char));
    if (!buf->data) {
        web_log(WEB_LOG_ERROR, "os_buf: buffer malloc error - %s", strerror(errno));
        os_free(buf);
        return NULL;
    }
    buf->len = len;
    buf->pos = buf->data;
    buf->end = buf->data + len;
    return buf;
}


void
os_buf_free(os_buf_t *buf)
{
    assert(buf);
    assert(buf->data);
    free(buf->data);
    free(buf);
}
