/*
 *
 * =====================================================================================
 *
 *       Filename:  event.h
 *
 *    Description:  
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */
#ifndef _EVENT_H_
#define _EVENT_H_

#include "queue.h"

typedef struct _event_data event_data_t;
typedef void (*ev_handler_t) (event_data_t *);

struct _event_data {
    void *data; 
    unsigned write   :1,
            accept  :1;
    struct timeval *timeout;
    int fd;
    int index; /* index to g_event_array */
    queue_t queue; /* posted events queue */
    ev_handler_t ev_handler;
};


typedef enum {
    EVENT_TYPE_READ,
    EVENT_TYPE_WRITE,
    EVENT_TYPE_EXCEPTION,
} event_type_t;

typedef enum {
    RT_OK = 0,
    RT_ERR   = -1,
    RT_BUSY  = -2,
    RT_AGAIN = -3,
} r_status_t;

/* interface between event core and downside layer, e.g. epoll/select module */
typedef struct _event_ops {
    r_status_t (*event_ops_add) (event_data_t *ev_data, event_type_t type);
    r_status_t (*event_ops_del) (event_data_t *ev_data, event_type_t type);
    r_status_t (*event_ops_dispatch) (void);
} event_ops_t;


typedef struct _event_module {
    char *name;
    /* module operations */
    void (*init_module) (void);
    void (*deinit_module) (void);
    event_ops_t ev_ops;
} event_module_t;

extern event_module_t *g_event_module_used;


/* 
 * TODO: move following APIs to a different file, as these APIs are 
 * exposed to upper layer
 */
/* event core APIs for upper layer (http module) */
#define event_add   g_event_module_used->ev_ops.event_ops_add
#define event_del   g_event_module_used->ev_ops.event_ops_del
#define event_dispatch g_event_module_used->ev_ops.event_ops_dispatch

void event_core_init(void);
void ev_post_event(event_data_t *ev_data);
void ev_process_posted_events(queue_t *queue);
void ev_process_all_events(void);


#endif
