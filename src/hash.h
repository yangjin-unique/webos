/*
 * =====================================================================================
 *
 *       Filename:  hash.h
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
#ifndef _HASH_H
#define _HASH_H
#include "common.h"


typedef ulong (*hash_func_t)(const void *);
typedef int (*hash_cmp_func_t)(const void *, const void *, size_t size);

typedef struct hash_node
{
	struct hash_node *prev; /* double circular linked list */
	struct hash_node *next; /* double circular linked list */
	char			*key;
	char			*value;
}hash_node_t;

typedef struct hash_tbl
{
	hash_node_t **bucket;
	uint32		size; /* table size */
	uint32		num_node; /* total # of nodes in table */
	hash_func_t		hash; /* hash function */
	hash_cmp_func_t hash_cmp;
}hash_tbl_t;


#endif
