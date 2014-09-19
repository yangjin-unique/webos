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


typedef long (*hash_func_t)(void *);
typedef int (*hash_cmp_func_t)(void *, void *);

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


/* public api */
hash_tbl_t *hash_tbl_init(uint32 size, hash_func_t hash, hash_cmp_func_t hash_cmp);
hash_node_t *hash_tbl_find(hash_tbl_t *htbl, char *key);
void hash_tbl_add(hash_tbl_t *htbl, char *key, char *value, int force_update);
void hash_tbl_del_by_key(hash_tbl_t *htbl, char *key);
void hash_tbl_del(hash_tbl_t *htbl, hash_node_t *node);
void print_hash_tbl(hash_tbl_t *htbl);
void hash_tbl_free_tbl(hash_tbl_t *htbl);
#endif
