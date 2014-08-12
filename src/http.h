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

typedef enum http_status_code 
{
	HTTP_CODE_OK					=	200,     /**< The request completed successfully */
	HTTP_CODE_BAD_REQUEST           =   400,     /**< The request is malformed */
	HTTP_CODE_NOT_FOUND             =   404,     /**< The requested resource was not found */
	HTTP_CODE_INTERNAL_SERVER_ERROR =   500,     /**< Server processing or configuration error. No response generated */

	HTTP_CODE_NOT_IMPLEMENTED       =   501,     /**< The server does not recognize the request or method */
	HTTP_CODE_SERVICE_UNAVAILABLE   =   503,     /**< The server is currently unavailable or overloaded */
	HTTP_CODE_BAD_VERSION           =   505,     /**< The server does not support the HTTP protocol version */
}http_status_code_t;

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

void http_parser_handler();
void http_parser_disconnect_handler();



#endif
