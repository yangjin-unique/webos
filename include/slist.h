/*
 * =====================================================================================
 *
 *       Filename:  slist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */
#ifndef _SLIST_H
#define _SLIST_H
#include <stdio.h>
#include <stdlib.h>

typedef struct slist_node
{
	struct slist_node *next;
}slist_node_t;


void slist_add_head(slist_node_t **head, slist_node_t *node);
void slist_add_tail(slist_node_t **head, slist_node_t *node);
void slist_remove(slist_node_t **head, slist_node_t *node);

#endif
