/*
 * =====================================================================================
 *
 *       Filename:  cgi.c
 *
 *    Description:  
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "cgi.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CGI_NUM_ENV_PAIR	20 /* support at leat 19 pairs */

int
is_cgi_req(char *uri)
{
	char *cgi_prefix = "/cgi";

	if (strncmp(uri, cgi_prefix, strlen(cgi_prefix)) == 0)
		return 1;
	return 0;
}


/* initialize cgi environmental params.
 * @return
 *		0: success;
 *		-1: error
 */
int
cgi_init_params(cgi_param_t **cgi, char *uri)
{
	char *path;
	char *deli;
	char *query_str = NULL;

	deli = strchr(uri, '?');
	path = uri + strlen("/cgi");
	if (deli != NULL)
	{
		*deli = '\0';
		query_str = deli + 1;
	}
	*cgi = (cgi_param_t *)malloc(sizeof(cgi_param_t));
	if (*cgi == NULL)
		return -1;
	memset(*cgi, 0, sizeof(cgi_param_t));
	(*cgi)->path = path;
	(*cgi)->query_string = query_str;
	(*cgi)->env = hash_tbl_init(29, NULL, NULL);
	(*cgi)->cgi_outfd = -1;
	(*cgi)->chld_pid = -1;
	if ((*cgi)->env == NULL)
	{
		free(*cgi);
		return -1;
	}
	return 0;
}


/* add key-value pair to cgi hash table */
void
cgi_add_env_pair(cgi_param_t *cgi, char *key, char *value)
{
	hash_tbl_add(cgi->env, key, value, 1);	
}


void
cgi_print_all_env(cgi_param_t *cgi)
{
	print_hash_tbl(cgi->env);
}


/* return value with given key */
char *
cgi_get_value(cgi_param_t *cgi, char *key)
{
	hash_node_t *node;

	node = hash_tbl_find(cgi->env, key);

	if (node == NULL)
		return NULL;
	return node->value;
}


/* add key-value pair to cgi env array */
void
add_one_pair_env(char **line, char *key, char *value)
{
	int len = 0;

	if (value == NULL)
		len = strlen(key) + 1 + 1;
	else
		len = strlen(key) + strlen(value) + 1 + 1;
	*line = (char *)malloc(len);
	if (*line == NULL)
		return;
	snprintf(*line, len, "%s=%s", key, value);
}

#define DEBUG
#ifdef DEBUG
void
cgi_print_env_array(char **env)
{
	printf("\n------------cgi env array----------\n");
	for (; *env != NULL; env++)
		printf("%s\n", *env);
}

#endif

/* build env array for cgi process */
char **
cgi_setup_env_array(cgi_param_t *cgi)
{
	char **env;
	char **p;
	env = (char **)malloc(CGI_NUM_ENV_PAIR * sizeof(char *));
	if (env == NULL)
		return env;
	p = env;
	add_one_pair_env(p++, "QUERY_STRING", cgi->query_string);
	add_one_pair_env(p++, "PATH_INFO", cgi->path);
	add_one_pair_env(p++, "REQUEST_URI", cgi_get_value(cgi, "uri"));
	add_one_pair_env(p++, "GATEWAY_INTERFACE", "CGI/1.1");
	add_one_pair_env(p++, "SERVER_PROTOCOL", "HTTP/1.1");
	add_one_pair_env(p++, "SERVER_SOFTWARE", "Webos/1.0");
	add_one_pair_env(p++, "SCRIPT_NAME", "/cgi");
	add_one_pair_env(p++, "REQUEST_METHOD", cgi_get_value(cgi, "http_method"));
	add_one_pair_env(p++, "CONTENT_LENGTH", cgi_get_value(cgi, "content-length"));
	add_one_pair_env(p++, "CONTENT_TYPE", cgi_get_value(cgi, "content-type"));
	add_one_pair_env(p++, "HTTP_ACCEPT", cgi_get_value(cgi, "accept"));
	add_one_pair_env(p++, "HTTP_REFERER", cgi_get_value(cgi, "referer"));
	add_one_pair_env(p++, "HTTP_ACCEPT_ENCODING", cgi_get_value(cgi, "accept-encoding"));
	add_one_pair_env(p++, "HTTP_ACCEPT_LANGUAGE", cgi_get_value(cgi, "accept-language"));
	add_one_pair_env(p++, "HTTP_ACCEPT_CHARSET", cgi_get_value(cgi, "accept-charset"));
	add_one_pair_env(p++, "HTTP_HOST", cgi_get_value(cgi, "host"));
	add_one_pair_env(p++, "HTTP_COOKIE", cgi_get_value(cgi, "cookie"));
	add_one_pair_env(p++, "HTTP_USER_AGENT", cgi_get_value(cgi, "user-agent"));
	add_one_pair_env(p++, "HTTP_CONNECTION", cgi_get_value(cgi, "connection"));
	*p = NULL; /* null terminated */
#ifdef DEBUG
	cgi_print_env_array(env);
#endif
	return env;
}


/* free cgi env memory */
void
cgi_free_env_array(char **env)
{
	char **p;
	if (env == NULL)
		return;
	for (p = env; *p != NULL; p++)
		free(*p);
	free(env);
}


/* free cgi hash table */
void
cgi_free_env_table(hash_tbl_t *env_tbl)
{
	if (env_tbl == NULL)
		return;

	hash_tbl_free_tbl(env_tbl);
}


