/*
 * =====================================================================================
 *
 *       Filename:  os.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/20/2015 06:55:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _OS_H_
#define _OS_H_

#define os_malloc   malloc
#define os_calloc   calloc
#define os_free     free


/* os buffer management */
typedef struct _os_buf_t {
    char *data;
    char *pos;
    char *end;
    int len;  
} os_buf_t;

os_buf_t *os_buf_malloc(int len);
void os_buf_free(os_buf_t *buf);

/* system related apis */
int os_set_nonblock(int fd);
int os_open_listen_sock(int port);




#endif
