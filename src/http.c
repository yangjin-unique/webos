/*
 * =====================================================================================
 *
 *       Filename:  http.c
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
#include <stdio.h>
#include "http.h"
#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "log.h"
#include "util.h"
#include <fcntl.h>

extern char g_www_root_folder[MAX_PATH_NAME];

char *
get_token(web_connection_t *conn, char *delim)
{
	char *token = NULL, *end_token = NULL, *next_token = NULL;
	char *end = conn->rbuf + conn->rsize;

	next_token = end;
	for (token = (char *)conn->prbuf; (*token == ' ' || *token == '\t') && (token < end);
						token++) 
	{ }
	
	if (delim == 0)
	{
		delim = " \t";
		if ((end_token = strpbrk(token, delim)) != 0)
		{
			*end_token = '\0';
			end_token++;
			while (*end_token == ' ' && *end_token == '\t') end_token++;
			next_token = end_token;
		}
	}
	else
	{
		if ((end_token = strstr(token, delim)) != 0)
		{
			*end_token = '\0';
			next_token = end_token + strlen(delim);
		}
		else
		{
			return NULL;
		}
	}
	/* move prbuf to next token */
	conn->prbuf = next_token;
	printf("prbuf=%x (%d)\n", (unsigned int)conn->prbuf, (conn->prbuf == (conn->rbuf+conn->rsize)));
	return token;
}


void
build_resp_status_line(web_connection_t *conn, http_resp_status_code_t code)
{
	switch (code)
	{
		case HTTP_CODE_OK:
			sprintf(conn->wbuf, "HTTP/1.1 %d OK\r\n", code);
			break;
		case HTTP_CODE_BAD_REQUEST:
			sprintf(conn->wbuf, "HTTP/1.1 %d BAD REQUEST\r\n", code);
			break;
		case HTTP_CODE_NOT_FOUND:
			sprintf(conn->wbuf, "HTTP/1.1 %d NOT FOUND\r\n", code);
			break;
		case HTTP_CODE_LENGTH_REQUIRED:
			sprintf(conn->wbuf, "HTTP/1.1 %d LENGTH REQUIRED\r\n", code);
			break;
		case HTTP_CODE_INTERNAL_SERVER_ERROR:
			sprintf(conn->wbuf, "HTTP/1.1 %d INTERNAL SERVER ERROR\r\n", code);
			break;
		case HTTP_CODE_SERVICE_UNAVAILABLE:
			sprintf(conn->wbuf, "HTTP/1.1 %d SERVICE UNAVAILABLE\r\n", code);
			break;
		case HTTP_CODE_BAD_VERSION:
			sprintf(conn->wbuf, "HTTP/1.1 %d HTTP VERSION NOT SUPPORT\r\n", code);
			break;
		case HTTP_CODE_NOT_IMPLEMENTED:
			sprintf(conn->wbuf, "HTTP/1.1 %d NOT IMPLEMENTED\r\n", code);
			break;
		default:
			sprintf(conn->wbuf, "HTTP/1.1 %d INTERNAL SERVER ERROR\r\n", code);
			break;
	}

	conn->wsize = strlen(conn->wbuf);
}

/* not thread-safe */
char *
get_time_str(time_t now)
{
	char *time;
	struct tm *ctm;

	if ((time = malloc(128)) == NULL)
	{
		web_log(WEB_LOG_ERROR, "time malloc failed\n");
		return NULL;
	}
	ctm = gmtime(&now);
	strftime(time, 128, "%a, %d %b %Y %H:%M:%S %Z", ctm);	
	return time;
}


size_t
read_nbytes(int fd, char *buf, size_t size)
{
	size_t n = 0, nbytesread = 0, nbytesleft = size;

	while (nbytesleft > 0)
	{
		n = read(fd, buf + nbytesread, nbytesleft);
		printf("read %d bytes, size=%d\n", n, size);
		if (n <= 0)
		{
			web_log(WEB_LOG_ERROR, "read failed\n");
			return -1;
		}
		nbytesread += n;
		nbytesleft -= n;
	}
	return nbytesread;
}


int
prepare_file(web_connection_t *conn, char *uri, http_resp_status_code_t *code)
{
	file_info_t *finfo;
	char path[MAX_PATH_NAME];
	struct stat stat_buf;
		
	if (uri == NULL)
		return -1;

	if ((finfo = malloc(sizeof(file_info_t))) == NULL)
	{
		web_log(WEB_LOG_ERROR, "file_info malloc failed\n");
		return -1;
	}
	memset(finfo, 0, sizeof(file_info_t));
	printf("www=%s\n", g_www_root_folder);
	strcpy(path, g_www_root_folder);
	if (*(path + strlen(path) -1) == '/')
		*(path + strlen(path) - 1) = '\0';

	strcat(path, uri);
	printf("get file = %s\n", path);

	if (stat(path, &stat_buf) < 0)
	{
		web_log(WEB_LOG_ERROR, "file fstat failed\n");
		*code = HTTP_CODE_NOT_FOUND;	
		free(finfo);
		return -1;
	}

	if (S_ISDIR(stat_buf.st_mode))
	{

		*(path + strlen(g_www_root_folder)) = '\0';
		strcat(path, "/index.html");	
		stat(path, &stat_buf);
		printf("get file = %s\n", path);
	}
	if ((finfo->fd = (open(path, O_RDONLY))) < 0)
	{
		web_log(WEB_LOG_ERROR, "can not open file %s\n", path);
		*code = HTTP_CODE_NOT_FOUND;	
		free(finfo);
		return -1;
	}
	
	finfo->size = (int) stat_buf.st_size;
	finfo->mtime = stat_buf.st_mtime;
	printf("file size = %d\n", finfo->size);

	conn->finfo = finfo;
	return 0;
}


void
load_file(web_connection_t *conn)
{
	file_info_t *finfo = conn->finfo;	

	if (conn->finfo == NULL)
		return;

	finfo->fbuf = (char *)malloc(finfo->size);
	if (finfo->fbuf == NULL)
	{
		web_log(WEB_LOG_ERROR, "file buffer malloc failed\n");
	}
	read_nbytes(finfo->fd, finfo->fbuf, finfo->size);
}

int
add_head_line(char *buf, char *key, char *value)
{
	char linestr[128];
	int len;

	sprintf(linestr, "%s:%s\r\n", key, value);
	len = strlen(linestr);
	memmove(buf, linestr, len);
	return len;
}

void 
build_resp_header(web_connection_t *conn)
{
	/* todo */
	char *pstr;
	char str[16];
	char *buf = conn->wbuf + conn->wsize;
	int len = 0, size = 0;
	
	if (conn->status == HTTP_PARSE_ERROR)
	{
		/* add connection field*/
		len = add_head_line(buf + size, "Connection", "close");
		size += len;
		/* todo: close connection */
	}
	else if (conn->status == HTTP_PARSE_END)
	{
		switch (conn->method)
		{
			case HTTP_METHOD_GET:
				load_file(conn);
			case HTTP_METHOD_HEAD:
				len = add_head_line(buf + size, "Connection", "keep-alive");
				size += len;
				snprintf(str, 16, "%d", conn->finfo->size);
				len = add_head_line(buf + size, "Content-length", str);
				size += len;
				break;
		}
	}

	pstr = get_time_str(time(NULL));
	if (pstr != NULL)
	{
		len = add_head_line(buf + size, "Date", pstr);
		size += len;
		free(pstr);
	}
	/* add server field */
	len = add_head_line(buf + size, "Server", "webos");
	size += len;
	
	sprintf(buf + size, "\r\n");/* header last line: \r\n */
	conn->wsize += size + 2;
}


void
http_send_resp(web_connection_t *conn)
{
	int nbytes;

	printf("send response now ...\n");
	if ((nbytes = send(conn->connfd, conn->wbuf, conn->wsize, MSG_DONTWAIT)) != conn->wsize)
	{
		web_log(WEB_LOG_ERROR, "send failed: only %d bytes sent\n", nbytes);
		return;
	}
	conn->wbuf[conn->wsize] = 0;
	printf("Response:\n%s\n", conn->wbuf);
	if (conn->finfo != NULL && conn->finfo->fbuf != NULL)
	{
		printf("send file now...");
		if ((nbytes = send(conn->connfd, conn->finfo->fbuf, 
						conn->finfo->size, MSG_DONTWAIT) != conn->finfo->size))
		{
			web_log(WEB_LOG_ERROR, "send file failed\n");
			free(conn->finfo->fbuf);
			return;
		}
	}
	return;
}


void
http_response(web_connection_t *conn, http_resp_status_code_t code)
{
	/* todo */

	build_resp_status_line(conn, code);
	build_resp_header(conn);
	http_send_resp(conn);
}


http_method_t
get_http_method(char *str)
{
	if (strcmp(str, "GET") == 0)
		return HTTP_METHOD_GET;
	else if (strcmp(str, "POST") == 0)
		return HTTP_METHOD_POST;
	else if (strcmp(str, "HEAD") == 0)
		return HTTP_METHOD_HEAD;
	else
		return HTTP_METHOD_UNKNOWN;
}


int
is_valid__http_proto_ver(char *str)
{
	int ver1, ver2;

	if (!str)
		return 0;
	if (sscanf(str, "HTTP/%d.%d", &ver1, &ver2) != 2)
		return 0;

	return (ver1 == 1 && ver2 == 1);
}


/* parse the request line: method, uri, http version */
http_resp_status_code_t
parse_request_line(web_connection_t *conn)
{
	char *method, *uri, *ver;
	http_resp_status_code_t code = HTTP_CODE_OK;

	/******* parse method *******/
	method = get_token(conn, 0);
	printf("method=%s\n", method);
	if (method == NULL || *method == '\0')
	{
		code = HTTP_CODE_BAD_REQUEST;
		goto err;
	}
	conn->method = get_http_method(method);
	if (conn->method == HTTP_METHOD_UNKNOWN)
	{
		/* todo */
		web_log(WEB_LOG_EVENT, "http method not impemented: %s\n", method);
		code = HTTP_CODE_NOT_IMPLEMENTED;
		goto err;
	}

	/*******  parse uri  *******/
	uri = get_token(conn, 0);
	printf("url=%s\n", uri);
	if (uri == NULL || *uri == '\0')
	{
		code = HTTP_CODE_BAD_REQUEST;
		goto err;
	}
	conn->uri = uri;
	if (prepare_file(conn, conn->uri, &code) == -1) /* check uri */
		goto err;

	/******** parse http version *******/
	ver = get_token(conn, "\r\n");
	printf("http version=%s\n", ver);
	if (ver == NULL || *ver == '\0')
	{
		code = HTTP_CODE_BAD_REQUEST;
		goto err;
	}
	if (!is_valid__http_proto_ver(ver))
	{
		code = HTTP_CODE_BAD_VERSION;
		goto err;
	}
	web_log(WEB_LOG_EVENT, "HTTP request, method=%s, uri=%s, http version=%s\n",
				method, uri, ver);
	
	conn->status = HTTP_PARSE_HEADER;

	//http_response(conn, HTTP_CODE_OK);
	return code;

err:	
	/* Todo: if need to close conn and free conn ???? */
	conn->status = HTTP_PARSE_ERROR;
	return code;
}


/* parse header line: general header, req header, entity header */
http_resp_status_code_t
parse_header(web_connection_t *conn)
{
	/* Todo */
	http_resp_status_code_t code = HTTP_CODE_OK;
	char *token, *end;
	char *key = NULL;
	char *value = NULL;
	int flag = 0;

	printf("...... parse header .....\n");
	
	while ((token = get_token(conn, "\r\n")) != NULL)
	{
		if (*token == '\0')
			break;

		if ((end = strchr(token, ':')) == NULL)
		{
			web_log(WEB_LOG_ERROR, "parse header error: token=%s\n", token);	
			code = HTTP_CODE_BAD_REQUEST;
			goto err;
		}
		key = token;
		*end = '\0';
		end++;
		while (*end == ' ' || *end == '\t') end++;
		value = end;
		web_log(WEB_LOG_EVENT, "%s=%s\n", key, value);

		if (str_caseless_cmp(key, "user-agent") == 0)
		{
			if ((conn->user_agent = malloc(strlen(value) + 1)) == NULL)
			{
				web_log(WEB_LOG_ERROR, "user-agent malloc failed\n");
				continue;
			}
			strcpy(conn->user_agent, value);
		}
		else if (str_caseless_cmp(key, "connection") == 0)
		{
			if (strcmp(value, "keep-alive") == 0)
				conn->conn_type = HTTP_CONN_TYPE_KEEPALIVE;
			else if (strcmp(value, "close") == 0)
				conn->conn_type = HTTP_CONN_TYPE_CLOSE;
			else
				conn->conn_type = HTTP_CONN_TYPE_UNKNOWN;
		}
		else if (str_caseless_cmp(key, "content-length") == 0)
		{
			conn->cont_len = atoi(value);
			flag = 1;
		}
	}
	
	if (conn->method == HTTP_METHOD_POST)
	{
		/* content length field */
		if (flag == 0)
			goto err;
	}


	if (conn->method == HTTP_METHOD_GET || conn->method == HTTP_METHOD_HEAD)
		conn->status = HTTP_PARSE_END; /* todo */
	else if (conn->method == HTTP_METHOD_POST)
		conn->status = HTTP_PARSE_CONTENT;
	else /* shoudle be impossible */
		conn->status = HTTP_PARSE_ERROR;
	return code;
err:
	conn->status = HTTP_PARSE_ERROR;
	return code;
}


void
parse_content(web_connection_t *conn)
{
	/* Todo */
	conn->status = HTTP_PARSE_END;
}


/* Todo: now not support pipeline req */
void
http_parser_handler(web_connection_t *conn)
{
	http_resp_status_code_t code = HTTP_CODE_OK;
	/* todo */
	web_log(WEB_LOG_EVENT, "http parser handler start....\n");
	conn->status = HTTP_PARSE_START;
	while (1) 
	{
		switch (conn->status)
		{
			case HTTP_PARSE_START:
				conn->status = HTTP_PARSE_REQ_LINE;
				break;
			case HTTP_PARSE_REQ_LINE:
				code = parse_request_line(conn);
				break;
			case HTTP_PARSE_HEADER:
				code = parse_header(conn);
				break;
			case HTTP_PARSE_CONTENT:
				parse_content(conn);
				break;
			case HTTP_PARSE_END:
			case HTTP_PARSE_ERROR:
				break;
		}

		if (conn->status == HTTP_PARSE_ERROR || conn->status == HTTP_PARSE_END)
		{/* todo */
			http_response(conn, code);
			break;
		}
	}
}

void
http_parser_disconnect_handler()
{
	/* todo */
}
