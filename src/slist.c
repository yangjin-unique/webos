/*
 * =====================================================================================
 *
 *       Filename:  slist.c
 *
 *    Description:  general single linked list implementation
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */
#include "slist.h"

/* add a node in the head of list  */
void
slist_add_head(slist_node_t **head, slist_node_t *node)
{
	if (node == NULL)
		return;

	node->next = *head;
	*head = node;
}

/* add a node in the tail of list */
void 
slist_add_tail(slist_node_t **head, slist_node_t *node)
{
	slist_node_t *curr_node = *head;
	if (node == NULL)
		return;
	if (*head == NULL)
	{
		*head = node;
		return;
	}
	while (curr_node->next != NULL)
		curr_node = curr_node->next;

	curr_node->next = node;
}

void
slist_remove(slist_node_t **head, slist_node_t *node)
{
	slist_node_t *curr_node;

	if (node == NULL || *head == NULL)
		return;

	if (*head == node)
	{
		*head = (*head)->next;
	}
	curr_node = *head;
	while (curr_node && curr_node->next && curr_node->next != node)
		curr_node = curr_node->next;
	if (curr_node && curr_node->next == node)
	{
		curr_node->next = curr_node->next->next;
		node->next = NULL;
	}
}

#if 0
typedef struct mylist
{
	struct mylist *next;
	char data;
}my_list_t;

void
print(my_list_t *head)
{
	while (head != NULL) 
	{
		printf("%c\t", head->data);
		head = head->next;
	}
	putchar('\n');
}


int
main()
{
	int i;
	my_list_t *head = NULL;
	my_list_t	 *node;
	printf("add node in head:\n");
	for (i=0; i<5; i++)
	{
		node = (my_list_t *)malloc(sizeof(my_list_t));
		node->data = 'a' + i;
		node->next = NULL;
		slist_add_head(&head, node);
	}
	print(head);
	printf("add node in tail:\n");
	for (i=0; i<5; i++)
	{
		node = (my_list_t *)malloc(sizeof(my_list_t));
		node->data = '1' + i;
		node->next = NULL;
		slist_add_tail(&head, node);
	}
	print(head);
	printf("remove node:\n");
	slist_remove(&head, head);
	print(head);
	slist_remove(&head, head->next->next);
	print(head);
	slist_remove(&head, head->next->next->next->next);
	print(head);
	return;
}

#endif
