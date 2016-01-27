/*
 * =====================================================================================
 *
 *       Filename:  epoll.c
 *
 *    Description:  epoll module that event based framework runs on 
 *
 *        Version:  2.0
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
#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>
#include "event.h"
#include "log.h"

#define EPOLL_SIZE      10*1024

static void epoll_module_init(void);
static void epoll_module_deinit(void);
static r_status_t epoll_add_event(event_data_t *ev_data); 
static r_status_t epoll_del_event(event_data_t *ev_data);
static r_status_t epoll_process_event(void);


event_module_t epoll_module = {
    .name = "Epoll",
    .init_module = epoll_module_init,
    .deinit_module = epoll_module_deinit,
    .ev_ops = {
        .event_ops_add = epoll_add_event,
        .event_ops_del = epoll_del_event,
        .event_ops_dispatch = epoll_process_event,
    },
};


/* epoll management entity */
typedef struct _epoll_mgmt {
    int epfd;
    int count;
    struct epoll_event *events;
} epoll_mgmt_t;


static epoll_mgmt_t g_epoll_mgmt;


static void 
epoll_module_init(void)
{
    int fd;

    memset(&g_epoll_mgmt, 0, sizeof(epoll_mgmt_t));
    g_epoll_mgmt.events = (struct epoll_event *) calloc(EPOLL_SIZE, sizeof(struct epoll_event));
    if (!g_epoll_mgmt.events) {
        web_log(WEB_LOG_ERROR, "Epoll module init failed (not enough memory)\n");
        goto ERROR;
    }

    fd = epoll_create(EPOLL_SIZE);
    if (fd < 0) {
        web_log(WEB_LOG_ERROR, "Epoll module init failed (%s)\n", strerror(errno));
        goto ERROR;
    }

    g_epoll_mgmt.epfd = fd;
    return;
ERROR:
    if (g_epoll_mgmt.events)
        free(g_epoll_mgmt.events);
    memset(&g_epoll_mgmt, 0, sizeof(epoll_mgmt_t));
}


static void
epoll_module_deinit(void)
{
    g_epoll_mgmt.epfd = -1;
    if (g_epoll_mgmt.events)
        free(g_epoll_mgmt.events);
    g_epoll_mgmt.events = NULL;
}



static r_status_t
epoll_add_event(event_data_t *ev_data)
{
    struct epoll_event event;

    assert(ev_data);
    if (g_epoll_mgmt.count > EPOLL_SIZE) {
        web_log(WEB_LOG_ERROR, "Epoll: failed to add event (events full)\n");
        return RT_ERR;
    }
    
    event.data.ptr = ev_data;
    event.events = EPOLLIN;

    if (epoll_ctl(g_epoll_mgmt.epfd, EPOLL_CTL_ADD, ev_data->fd, &event) < 0) {
        web_log(WEB_LOG_ERROR, "Epoll: failed to add event (%s)\n", strerror(errno));
        return RT_ERR;
    }
    g_epoll_mgmt.count++;
    web_log(WEB_LOG_DEBUG, "Epoll: add an event, fd=%d, cout=%d\n", ev_data->fd, g_epoll_mgmt.count);
    return RT_OK;
}


static r_status_t
epoll_del_event(event_data_t *ev_data)
{
    struct epoll_event event;

    assert(ev_data);
    event.events = EPOLLIN;
    event.data.ptr = ev_data;
   
    if (epoll_ctl(g_epoll_mgmt.epfd, EPOLL_CTL_DEL, ev_data->fd, &event) < 0) {
        web_log(WEB_LOG_ERROR, "Epoll: failed to delete an event (%s)\n", strerror(errno));
        return RT_ERR;
    }
    g_epoll_mgmt.count--;
    web_log(WEB_LOG_DEBUG, "Epoll: delete an event, fd=%d, cout=%d\n", ev_data->fd, g_epoll_mgmt.count);
    return RT_OK;
}


static r_status_t
epoll_process_event(void)
{
    int i;
    int nready;
    event_data_t *ev_data = NULL;

    nready = epoll_wait(g_epoll_mgmt.epfd, g_epoll_mgmt.events, EPOLL_SIZE, -1);
    if (nready < 0) {
        web_log(WEB_LOG_ERROR, "Epoll: epoll_wait error (%s)\n", strerror(errno));
        return RT_ERR;
    }
    else if (nready > 0) {
        for (i = 0; i < nready; i++) {
            if (g_epoll_mgmt.events[i].events & EPOLLIN) {
                ev_data = (event_data_t *) g_epoll_mgmt.events[i].data.ptr; 
                ev_post_event(ev_data);
            }
        }
    }
    return RT_OK;
}


