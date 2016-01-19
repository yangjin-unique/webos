/*
 *
 * =====================================================================================
 *
 *       Filename:  core.c
 *
 *    Description: core loop engine  
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
#include "event.h"
#include "listen.h"
#include "connection.h"
#include "core.h"
#include "log.h"

int g_termin_service = 0;

void
core_engine_init(web_engine_t *engine, int http_port, int https_port)
{
    connection_init(128);
    listen_init(&engine->http, http_port); 
    listen_init(&engine->https, https_port); 
}

void
core_engine_run(void)
{
    for (; ;) {
        if (g_termin_service) {
            break;
        }
        web_log(WEB_LOG_EVENT, "core event dispatch\n");
        event_dispatch();
        web_log(WEB_LOG_EVENT, "core events processing\n");
        ev_process_all_events();
    }
}
