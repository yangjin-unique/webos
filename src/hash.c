/*
 * =====================================================================================
 *
 *       Filename:  hash.c
 *
 *    Description:  string hash table implementation
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
#include <stdio.h>
#include <string.h>
#include "hash.h"

#define HASH_TBL_DEFAULT_SIZE	63

/* default hash function: djb2 hash function */
ulong
hash_default(void *str)
{
	char *s = str;
	ulong hash = 5381;
	int c;

	while ((c = *s++))
		hash += (hash<<5) + c; 
	return hash;
}


/* default hash key cmpare function */
int
hash_cmp_default(void *s1, void *s2)
{
	return strcmp(s1, s2);
}


/* creat a hash table entry */
hash_tbl_t *
hash_tbl_init(uint32 size, hash_func_t hash, hash_cmp_func_t hash_cmp)
{
	hash_tbl_t *htbl;

	htbl = malloc(sizeof(hash_tbl_t));
	if (htbl == NULL)
		return NULL;

	memset(htbl, 0, sizeof(hash_tbl_t));
	htbl->size = (size == 0) ? HASH_TBL_DEFAULT_SIZE : size;
	htbl->hash = (hash == NULL) ? (hash_func_t)hash_default : hash;
	htbl->hash_cmp = (hash_cmp == NULL) ? (hash_cmp_func_t)hash_cmp_default : hash_cmp;
	htbl->bucket = (hash_node_t **)calloc(htbl->size, sizeof(hash_node_t *));
	if (htbl->bucket == NULL)
	{
		free(htbl);
		return NULL;
	}
	return htbl;	
}


/* alloc a new hash node */
hash_node_t *
hash_node_alloc(char *key, char *value)
{
	hash_node_t *node;

	node = malloc(sizeof(hash_node_t));
	if (node == NULL)
		return NULL;

	memset(node, 0, sizeof(hash_node_t));
	node->key = malloc(strlen(key)+1);
	if (node->key == NULL)
	{
		free(node);
		return NULL;
	}
	memset(node->key, 0, strlen(key)+1);
	strcpy(node->key, key);
	node->value = malloc(strlen(value)+1);
	if (node->value == NULL)
	{
		free(node->key);
		free(node);
		return NULL;
	}
	memset(node->value, 0, strlen(value)+1);
	strcpy(node->value, value);
	return node;
}


void
hash_node_free(hash_node_t *node)
{
	if (node == NULL)
		return;
	if (node->value != NULL)
		free(node->value);
	if (node->key != NULL)
		free(node->key);
	free(node);
}

hash_node_t *
hash_tbl_find(hash_tbl_t *htbl, char *key)
{
	hash_node_t *node;
	ulong index;

	index = htbl->hash(key) % htbl->size;
	node = htbl->bucket[index];

	if (node == NULL)
		return NULL;
	if (htbl->hash_cmp(node->key, key) == 0)
		return node;
	node = node->next;
	while (node != htbl->bucket[index])
	{
		if (htbl->hash_cmp(node->key, key) == 0)
			return node;
		node = node->next;
	}
	return NULL;
}

/* add a node to hash table 
 * If a key already existed in table, [force_update=1] would update the value
 * of the key.
 */
void
hash_tbl_add(hash_tbl_t *htbl, char *key, char *value, int force_update)
{
	hash_node_t *node;
	hash_node_t *head;
	ulong index;
	size_t len;

	if (htbl == NULL || key == NULL) /* value can be null */
		return;
	if ((node = hash_tbl_find(htbl, key)) != NULL)
	{
		if (force_update == 1)
		{
			if (strcmp(node->value, value) == 0)	
				return; /* same value */
			/* update the existed node */
			if ((len = strlen(value)) > strlen(node->value))
			{
				node->value = realloc(node->value, len+1);
				memset(node->value, 0, len+1);
				strncpy(node->value, value, len+1);
			}
			else
				strcpy(node->value, value);
		}
		return; /* already existed */
	}
	
	/* add a new node */
	node = hash_node_alloc(key, value);

	index = htbl->hash(key) % htbl->size;
	head = htbl->bucket[index];
	if (head == NULL)
	{
		htbl->bucket[index] = node;
		node->prev = node;
		node->next = node;
	}
	else
	{
		node->next = head; /* first update new node pointer */
		node->prev = head->prev;
		head->prev->next = node; /* then modify two adjacent nodes pointer */
		head->prev = node;
		htbl->bucket[index] = node;
	}
	htbl->num_node++;
}


void
hash_tbl_del(hash_tbl_t *htbl, hash_node_t *node)
{
	if (htbl == NULL ||node == NULL)
		return;

	node->prev->next = node->next;
	node->next->prev = node->prev;
	hash_node_free(node);
	htbl->num_node--;
}


void
hash_tbl_del_by_key(hash_tbl_t *htbl, char *key)
{
	hash_node_t *node;

	node = hash_tbl_find(htbl, key);
	if (node == NULL)
		return;
	hash_tbl_del(htbl, node);
	htbl->num_node--;
}


void
hash_tbl_free_tbl(hash_tbl_t *htbl)
{
	int i;
	hash_node_t *node, *prev;

	if (htbl == NULL)
		return;
	for (i = 0; i < htbl->size; i++)
	{
		prev = node = htbl->bucket[i];
		if (node == NULL)
			continue;
		else
		{
			node = node->next;
			free(prev);
		}

		while (node != htbl->bucket[i])
		{
			prev = node;
			node = node->next;
			free(prev);
			prev = NULL;
		}
	}
	free(htbl);
}

void
print_hash_tbl(hash_tbl_t *htbl)
{
	hash_node_t *node;
	int i;

	printf("\n|-----------------------hash tbl----------------------------|\n");
	for (i=0; i < htbl->size; i++)
	{
		printf("%d: ", i);
		node = htbl->bucket[i];
		if (node == NULL)
		{
			printf("NULL\n");
			continue;
		}
		printf("[%s %ld %s]\t", node->key, htbl->hash(node->key), node->value);
		node = node->next;
		while (node != htbl->bucket[i])
		{
			printf("[%s %ld %s]\t", node->key, htbl->hash(node->key), node->value);
			node = node->next;
		}
		putchar('\n');
	}
	printf("|-----------------------end of hash tbl-------------------------|\n\n");
}


#ifdef DEBUG
int
main()
{
	char key[32];
	char value[32];
	//hash_node_t *node;
	hash_tbl_t *htbl;

	memset(key, 0, sizeof(key));
	memset(value, 0, sizeof(value));

	htbl = hash_tbl_init(17, NULL, NULL);
	hash_tbl_add(htbl, "xing", "yang", 1);
	hash_tbl_add(htbl, "ming", "jin", 1);

	hash_tbl_add(htbl, "email", "yangjin@gmai.com", 1);
	hash_tbl_add(htbl, "age", "25", 1);
	hash_tbl_add(htbl, "age", "30", 1);

	hash_tbl_add(htbl, "gender", "man", 1);
	
	hash_tbl_add(htbl, "address", "King Road", 1);

	hash_tbl_add(htbl, "phone", "021-99999999", 1);

	hash_tbl_add(htbl, "number", "1111", 1);
	print_hash_tbl(htbl);
	return 0;
}
#endif
