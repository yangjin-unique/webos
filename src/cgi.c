/*
 * =====================================================================================
 *
 *       Filename:  cgi.c
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
#include "cgi.h"
#include <string.h>

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
	(*cgi)->env = hash_tbl_init(17, NULL, NULL);
	if ((*cgi)->env == NULL)
	{
		free(*cgi);
		return -1;
	}
	return 0;
}


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

char *
cgi_get_value(cgi_param_t *cgi, char *key)
{
	hash_node_t *node;

	node = hash_tbl_find(cgi->env, key);

	if (node == NULL)
		return NULL;
	return node->value;
}


char *
add_one_pair_env(char *key, char *value)
{
	char *line;
	int len = strlen(key) + strlen(value) + 1 + 1;
	line = (char *)malloc(len);
	if (line == NULL)
		return NULL;
	snprintf(line, len, "%s=%s", key, value);
	return line;
}

char **
cgi_setup_env(cgi_param_t *cgi)
{
	char **env;
	int i = 0;
	char *http_method[] = {"GET", "HEAD", "POST", "UNKNOWN"};

	env = (char **)malloc(sizeof(char *));
	add_one_pair_env(&env[i++], "QUERY_STRING", cgi->query_string);
	add_one_pair_env(&env[i++], "PATH_INFO", cgi->path);

	add_one_pair_env(&env[i++], "REQUEST_URI", cgi_get_value(cgi, "uri"));
	add_one_pair_env(&env[i++], "GATEWAY_INTERFACE", "CGI/1.1");
	add_one_pair_env(&env[i++], "SERVER_PROTOCOL", "HTTP/1.1");
	add_one_pair_env(&env[i++], "SERVER_SOFTWARE", "Webos/1.0");
	add_one_pair_env(&env[i++], "SCRIPT_NAME", "/cgi");
	add_one_pair_env(&env[i++], "REQUEST_METHOD", cgi_get_value(cgi, "http_method"));
	add_one_pair_env(&env[i++], "CONTENT_LENGTH", cgi_get_value(cgi, "content-length"));
	add_one_pair_env(&env[i++], "CONTENT_TYPE", cgi_get_value(cgi, "content-type"));
}

