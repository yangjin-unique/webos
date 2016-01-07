#include <unistd.h>
#include <sys/type.h>
#include <sys/time.h>
#include "event.h"

#define MAX_NUM_OF_EVENT    2048 /* tunnable */

static char *module_name = "Select";


static void select_module_init(void);
static void select_module_deinit(void);
static r_status_t select_add_event(event_data_t *ev_data, event_type_t type); 
static r_status_t select_del_event(event_data_t *ev_data, event_type_t type);
static r_status_t select_process_event(void);


static event_ops_t select_ops = {
    .event_add = select_add_event;
    .event_del = select_del_event;
    .event_dispatch = select_process_event;
};

static event_module_t select_module =*{
    .name = module_name;
    .init_module = select_module_init;
    .deinit_module = select_module_deinit;
    .ev_ops = select_ops;
};

static fd_set g_select_rset;
static fd_set g_select_wset;
static fd_set g_select_excpset;

static event_data_t *g_event_array[MAX_NUM_OF_EVENT];
static int g_num_events = 0;

static void
select_module_init(void)
{
    FD_ZERO(&g_select_rset);
    FD_ZERO(&g_select_wset);
    g_num_events = 0;
}


static void
select_module_deinit(void)
{
    FD_ZERO(&g_select_rset);
    FD_ZERO(&g_select_wset);
    g_num_events = 0;
}

static r_status_t
select_add_event(event_data_t *ev_data, event_type_t type)
{
    int fd = ev_data->connfd;

    swtitch (type) {
    case EVENT_TYPE_READ:
        FD_SET(fd, g_select_rset);
        break;
    case EVENT_TYPE_WRITE:
        FD_SET(fd, g_select_wset);
        break;
    case EVENT_TYPE_EXCEPTION:
        FD_SET(fd, g_select_excpset);
        break;
    default:
        break;
    }
    if (g_num_events > MAX_NUM_OF_EVENT) {
        web_log(WEB_LOG_ERROR, "error: events full");
        return R_ERR;
    }
    if (fd > g_max_fd)
        g_max_fd = fd;

    g_event_array[g_num_events] = ev_data; 
    ev_data->index = g_num_events;
    g_num_events++;
    return R_OK;
}


/**
 * @brief delete an event, and move the last event to the deleted event 
 *  position.
 */
static r_status_t
select_del_event(event_data_t *ev_data, event_type_t type)
{
    int index = ev_data->index;

    assert(index >= 0 && index <= MAX_NUM_OF_EVENT);
    g_num_events--;
    g_event_array[index] = g_event_array[g_num_events];
    g_event_array[index]->index = index;
    ev_data->index = -1;
}


static r_status_t
select_process_event(void)
{
    int i;
    int maxfd = 0;
    int nready;
    struct timeval timeout;
    event_data_t *ev_data = NULL;

    for (i = 0; i < g_num_events; i++) {
        if (g_event_array[i]->fd > maxfd) 
            maxfd = g_event_array[i]->fd;
    }
    maxfd += 1;
    
    nready = select(maxfd, &g_select_rset, &g_select_wset, &g_select_excpset, NULL);

    if (nready == -1) {
        char buf[ERROR_STR_LEN] = {0};
        web_log(WEB_LOG_ERROR, "%s error: %s", module_name, strerror(errno)i, buf, ERROR_STR_LEN);
        return R_ERR;
    } 
    else if (nready == 0) {
        /* TODO: timeout */
    }
    else {
        for (i = 0; i < g_num_events && nready > 0; i++) {
            ev_data = g_event_array[i];
            if (ev_data->write) {
                if (FD_ISSET(ev_data->fd, g_select_wset)) {
                    found = 1;
                }
            }
            else {
                if (FD_ISSET(ev_data->fd, g_select_rset)) {
                    found = 1;
                }
            }
            if (found == 1) {
                ev_post_event(ev_data);
                nready--;
            }
        }
    }
}

