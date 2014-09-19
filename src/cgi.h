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
#include <unistd.h>

typedef struct cgi_param
{
	/* todo */
	hash_tbl_t	*env; /* cgi environmental variables */
	char		*path;
	char		*query_string;
	pid_t		chld_pid; /* cgi child process pid */
	int			cgi_outfd; /* used to recv child process output */
}cgi_param_t;

int is_cgi_req(char *uri);
int cgi_init_params(cgi_param_t **cgi, char *uri);
void cgi_add_env_pair(cgi_param_t *cgi, char *key, char *value);
void cgi_print_all_env(cgi_param_t *cgi);
char *cgi_get_value(cgi_param_t *cgi, char *key);
char **cgi_setup_env_array(cgi_param_t *cgi);
void cgi_free_env_array(char **env);
void cgi_free_env_table(hash_tbl_t *env);

#endif
