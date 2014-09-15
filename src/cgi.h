/*
 * =====================================================================================
 *
 *       Filename:  cgi.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _CGI_H
#define _CGI_H
#include "hash.h"

typedef struct cgi_param
{
	/* todo */
	hash_tbl_t	*env; /* cgi environmental variables */
	char		*path;
	char		*query_string;
}cgi_param_t;

int is_cgi_req(char *uri);
int cgi_init_params(cgi_param_t *cgi);
void cgi_add_env_pair(cgi_param_t *cgi, char *key, char *value);
void cgi_print_all_env(cgi_param_t *cgi);
char *cgi_get_value(cgi_param_t *cgi, char *key);

#endif
