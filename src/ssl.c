/*
 * =====================================================================================
 *
 *       Filename:  ssl.c
 *
 *    Description:  ssl support
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#include "ssl.h"
#include "log.h"

SSL_CTX *
ssl_init(char *key, char *crt)
{
	SSL_CTX *ssl_context;

	SSL_load_error_strings();
	SSL_library_init();

	/* use TLSv1 only */
	if ((ssl_context = SSL_CTX_new(TLSv1_server_method())) == NULL)
	{
		web_log(WEB_LOG_ERROR, "create SSL context failed");
		return NULL;
	}
	/* register private key */
	if (SSL_CTX_use_PrivateKey_file(ssl_context, key, SSL_FILETYPE_PEM) == 0)
	{
		SSL_CTX_free(ssl_context);
		web_log(WEB_LOG_ERROR, "load private key failed");
		return NULL;
	}
	/* register public crt */
	if (SSL_CTX_use_certificate_file(ssl_context, crt, SSL_FILETYPE_PEM) == 0)
	{
		SSL_CTX_free(ssl_context);
		web_log(WEB_LOG_ERROR, "load certificate failed");
		return NULL;
	}

	web_log(WEB_LOG_EVENT, "SSL library initialized");
	return ssl_context;
}
