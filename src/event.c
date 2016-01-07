#include <stdlib.h>
#include "queue.h"
#include "event.h"

#define USE_SELECT_MODULE   1

event_module_t *g_event_module_used = NULL;

static queue_t g_posted_events;
static queue_t g_posted_accept_events;

#if USE_SELECT_MODULE
extern event_module_t *select_module;
#endif

void
event_core_init(void)
{
#if USE_EPOLL_MODULE
    g_event_module_used = epoll_module;
#elif USE_SELECT_MODULE
    g_event_module_used = select_module;
#endif
    g_event_module_used->init_module();

    queue_init(&g_posted_events);
    queue_init(&g_posted_accept_events);
}


/**
 * @brief called by down layer, e.g. select/epoll module
 * when events happen, we should post the event to queue 
 * for later process.
 * @param ev_data
 */
void
ev_post_event(event_data_t *ev_data)
{
   queue_t *queue;

   queue = ev_data->accept ? &g_posted_accept_events : 
                            &g_posted_events;
   queue_insert_tail(queue, &ev_data->queue);
}


/**
 * @brief   event core APIs
 */
void
ev_process_posted_events(queue_t *queue)
{
    event_data_t *ev_data;
    queue_t *q;
    /* process pending accept events */
    while (!queue_empty(queue)) {
        q = queue_head(&g_posted_accept_events);
        ev_data = queue_entry(q, event_data_t, queue);
        ev_data->ev_handler(ev_data);
        queue_del(q);
    }
}


/**
 * @brief process all pending events
 */
void
ev_process_all_events(void)
{
    ev_process_posted_events(&g_posted_accept_events);
    ev_process_posted_events(&g_posted_events);
}
