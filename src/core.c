#include "event.h"
#include "listen.h"
#include "core.h"

void
core_engine_init(web_engine_t *engine, int http_port, int https_port)
{
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
        printf("core event dispatch\n");
        event_dispatch();
        printf("core events processing\n");
        ev_process_all_events();
    }
}
