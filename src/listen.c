/*
 * =====================================================================================
 *
 *       Filename:  listen.c
 *
 *    Description: listen module 
 *
 *        Version:  1.0
 *        Created:  10/19/2015 06:51:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include "listen.h"
#include "log.h"
#include "os.h"
#include "connection.h"

int g_num_conn = 0;

static void
listen_event_handler(event_data_t *ev_data)
{
    listen_opt_t *opt = (listen_opt_t *) ev_data->data;
    int fd;
    struct sockaddr_in client;
    socklen_t slen = sizeof(client);
    
    memset(&client, 0, slen);
    web_log(WEB_LOG_DEBUG, "Listrn: listen event handler\n");
    fd = accept(opt->fd, (struct sockaddr*) &client, &slen);
    if (fd < 0) {
        web_log(WEB_LOG_ERROR, "Listen: accept failed listenfd=%d (%s)\n", opt->fd, strerror(errno));
    }
    os_set_nonblock(fd);
    g_num_conn++;
    web_log(WEB_LOG_DEBUG, "Listen: accept a connection: fd=%d (total conn=%d)\n", fd, g_num_conn);
    if (fd < 0) {
        web_log(WEB_LOG_ERROR, "listen: accept error - %s", strerror(errno));
        return;
    }
    connection_add(fd); /* TODO: connection module */
}


void
listen_init(listen_opt_t *opt, int port)
{
    assert(port > 0);    
    assert(opt);

    opt->port = port;
    opt->fd = os_open_listen_sock(port);
    web_log(WEB_LOG_DEBUG, "Lister fd=%d\n", opt->fd);
    opt->ev_data.data = opt;
    opt->ev_data.accept = 1;
    opt->ev_data.write = 0; /* read event */
    opt->ev_data.fd = opt->fd;
    opt->ev_data.ev_handler = listen_event_handler;
    /* add listen event */
    event_add(&opt->ev_data);
    return;
}


