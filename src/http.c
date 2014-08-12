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
#include "log.h"

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
	}
	/* move prbuf to next token */
	conn->prbuf = next_token;
	printf("prbuf=%x (%d)\n", (unsigned int)conn->prbuf, (conn->prbuf == (conn->rbuf+conn->rsize)));
	return token;
}



void
build_resp_status_line(web_connection_t *conn, http_status_code_t code)
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



void
http_send(web_connection_t *conn)
{
	int nbytes;

	if ((nbytes = send(conn->connfd, conn->wbuf, conn->wsize, MSG_DONTWAIT)) != conn->wsize)
	{
		web_log(WEB_LOG_ERROR, "send failed: only %d bytes sent\n", nbytes);
	}

	return;
}


void
http_response(web_connection_t *conn, http_status_code_t code)
{
	/* todo */

	build_resp_status_line(conn, code);
	http_send(conn);
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

/* parse the request line:  */
void
parse_request_line(web_connection_t *conn)
{
	char *method, *uri, *ver;

	/* parse method */
	method = get_token(conn, 0);
	printf("method=%s\n", method);
	if (method == NULL || *method == '\0')
		goto bad_request;

	conn->method = get_http_method(method);
	if (conn->method == HTTP_METHOD_UNKNOWN)
	{
		/* todo */
		web_log(WEB_LOG_EVENT, "http method not impemented: %s\n", method);
		http_response(conn, HTTP_CODE_NOT_IMPLEMENTED);
		return;
	}

	/* parse uri */
	uri = get_token(conn, 0);
	printf("token-url=%s\n", uri);
	if (uri == NULL || *uri == '\0')
		goto bad_request;

	conn->uri = uri;

	/* parse http version */
	ver = get_token(conn, "\r\n");
	printf("http version=%s\n", ver);
	
	web_log(WEB_LOG_EVENT, "HTTP request, method=%s, uri=%s, http version=%s\n",
				method, uri, ver);
	
	conn->status = HTTP_PARSE_HEADER;

	http_response(conn, HTTP_CODE_OK);
	return;

bad_request:	
	/* Todo: if need to close conn and free conn ???? */
	http_response(conn, HTTP_CODE_BAD_REQUEST);
	conn->status = HTTP_PARSE_ERROR;
	return;
}


void
parse_header(web_connection_t *conn)
{
	/* Todo */
}


void
parse_content(web_connection_t *conn)
{
	/* Todo */
}


/* Todo: now not support pipeline req */
void
http_parser_handler(web_connection_t *conn)
{
	/* todo */
	while (1) 
	{
		switch (conn->status)
		{
			case HTTP_PARSE_START:
				conn->status = HTTP_PARSE_REQ_LINE;
				break;
			case HTTP_PARSE_REQ_LINE:
				parse_request_line(conn);
				break;
			case HTTP_PARSE_HEADER:
				parse_header(conn);
				break;
			case HTTP_PARSE_CONTENT:
				parse_content(conn);
				break;
			case HTTP_PARSE_END:
				break;
		}

		if (conn->status == HTTP_PARSE_ERROR)
		{/* todo */
			break;
		}
	}
}

void
http_parser_disconnect_handler()
{
	/* todo */
}
