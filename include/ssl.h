/*
 * =====================================================================================
 *
 *       Filename:  ssl.c
 *
 *    Description:  ssl support
 *
 *        Version:  2.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#ifndef _SSL_H
#define _SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>
/* public api */
SSL_CTX *ssl_init(char *key, char *cert);

#endif

