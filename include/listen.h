/*
 * =====================================================================================
 *
 *       Filename:  listen.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/19/2015 06:51:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _LISTEN_H_
#define _LISTEN_H_

#include "event.h"

typedef struct _listen_opt_t listen_opt_t;

struct _listen_opt_t {
    int port; /* listen port */
    int fd; /* listen fd */
    event_data_t ev_data; /* listen for accept event */
};

void listen_init(listen_opt_t *opt, int port);







#endif
