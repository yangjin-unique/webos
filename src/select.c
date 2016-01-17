/*
 *
 * =====================================================================================
 *
 *       Filename:  select.c
 *
 *    Description: select module that event based framework runs on  
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "event.h"
#include "log.h"

#define MAX_NUM_OF_EVENT    2048 /* tunnable */
#define ERROR_STR_LEN       128


static void select_module_init(void);
static void select_module_deinit(void);
static r_status_t select_add_event(event_data_t *ev_data); 
static r_status_t select_del_event(event_data_t *ev_data);
static r_status_t select_process_event(void);


event_module_t select_module = {
    .name = "Select",
    .init_module = select_module_init,
    .deinit_module = select_module_deinit,
    .ev_ops = {
        .event_ops_add = select_add_event,
        .event_ops_del = select_del_event,
        .event_ops_dispatch = select_process_event,
    },
};


static event_data_t *g_event_array[MAX_NUM_OF_EVENT];
static int g_num_events = 0;

static void
select_module_init(void)
{
    g_num_events = 0;
}


static void
select_module_deinit(void)
{
    g_num_events = 0;
}

static r_status_t
select_add_event(event_data_t *ev_data)
{
    if (g_num_events > MAX_NUM_OF_EVENT) {
        web_log(WEB_LOG_ERROR, "error: events full");
        return RT_ERR;
    }
    g_event_array[g_num_events] = ev_data; 
    ev_data->index = g_num_events;
    g_num_events++;
    web_log(WEB_LOG_EVENT, "Select: add a event, fd=%d, g_num_events=%d\n", ev_data->fd, g_num_events);
    return RT_OK;
}


/**
 * @brief delete an event, and move the last event to the deleted event 
 *  position.
 */
static r_status_t
select_del_event(event_data_t *ev_data)
{
    int index = ev_data->index;

    assert(index >= 0 && index <= MAX_NUM_OF_EVENT);
    g_num_events--;
    g_event_array[index] = g_event_array[g_num_events];
    g_event_array[index]->index = index;
    ev_data->index = -1;
    web_log(WEB_LOG_EVENT, "Select: del a event, g_num_events=%d\n", g_num_events);
    return RT_OK;
}


static r_status_t
select_process_event(void)
{
    int i;
    int maxfd = 0;
    int nready;
    event_data_t *ev_data = NULL;
    int found = 0;
    fd_set rset;

    FD_ZERO(&rset);
    printf("------------------Select---------------\n");
    web_log(WEB_LOG_EVENT, "Total # fds: %d\n", g_num_events);
    for (i = 0; i < g_num_events; i++) {
        printf("fd=%d\n", g_event_array[i]->fd);
        FD_SET(g_event_array[i]->fd, &rset);
        if (g_event_array[i]->fd > maxfd) 
            maxfd = g_event_array[i]->fd;
    }
    printf("------------------End------------------\n\n");
    maxfd += 1;
    web_log(WEB_LOG_EVENT, "Select: process event...\n"); 
    nready = select(maxfd, &rset, NULL, NULL, NULL);
    
    web_log(WEB_LOG_EVENT, "Select: process event: %d events ready\n", nready); 
    if (nready == -1) {
        char buf[ERROR_STR_LEN] = {0};
        web_log(WEB_LOG_ERROR, "select error: %s", strerror(errno), buf, ERROR_STR_LEN);
        return RT_ERR;
    }
    else if (nready > 0) {
        for (i = 0; i < g_num_events && nready > 0; i++) {
            ev_data = g_event_array[i];
           
            if (FD_ISSET(ev_data->fd, &rset))
                found = 1;
            
            if (found) {
                ev_post_event(ev_data);
                nready--;
            }
        }
    }
    return RT_OK;
}

