/*
 * =====================================================================================
 *
 *       Filename:  http.h
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
#ifndef _HTTP_H
#define _HTTP_H

#include "connection.h"

#define MAX_PATH_NAME	256

typedef enum http_resp_status_code 
{
	HTTP_CODE_OK					=	200,     /* The request completed successfully */
	HTTP_CODE_BAD_REQUEST           =   400,     /* The request is malformed */
	HTTP_CODE_NOT_FOUND             =   404,     /* The requested resource was not found */
	HTTP_CODE_LENGTH_REQUIRED			=   411,     /* length required for POSTs */
	HTTP_CODE_INTERNAL_SERVER_ERROR =   500,     /* Server processing or configuration error. No response generated */

	HTTP_CODE_NOT_IMPLEMENTED       =   501,     /* The server does not recognize the request or method */
	HTTP_CODE_SERVICE_UNAVAILABLE   =   503,     /* The server is currently unavailable or overloaded */
	HTTP_CODE_BAD_VERSION           =   505,     /* The server does not support the HTTP protocol version */
}http_resp_status_code_t;

typedef enum http_parse_status
{	
	HTTP_PARSE_START,
	HTTP_PARSE_REQ_LINE,
	HTTP_PARSE_HEADER,
	HTTP_PARSE_CONTENT,
	HTTP_PARSE_ERROR,
	HTTP_PARSE_END,
}http_parse_status_t;


typedef enum http_method
{
	HTTP_METHOD_GET,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,
	HTTP_METHOD_UNKNOWN,
}http_method_t;

enum http_conn_type
{
	HTTP_CONN_TYPE_NONE,
	HTTP_CONN_TYPE_KEEPALIVE,
	HTTP_CONN_TYPE_CLOSE,
	HTTP_CONN_TYPE_UNKNOWN,
};

typedef enum http_content_type
{
	HTTP_CONTENT_TYPE_HTML,
	HTTP_CONTENT_TYPE_CSS,
	HTTP_CONTENT_TYPE_JPEG,
	HTTP_CONTENT_TYPE_PNG,
	HTTP_CONTENT_TYPE_GIF,
	HTTP_CONTENT_TYPE_OTHER,
}http_content_type_t;

void http_parser_handler(web_connection_t *conn);
void http_parser_disconnect_handler(web_connection_t *conn);


#endif
