/*
 * =====================================================================================
 *
 *       Filename:  webos.c
 *
 *    Description:  main web server exec routine
 *
 *        Version:  1.0
 *        Created:  07/23/2014 07:01:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *			Usage:	./webos <port> <log_file> <lock_file>  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "web_engine.h"
#include "http.h"
#include "log.h"
#include "string.h"
#define USAGE	"\nUsage: %s <http_port> <https_port> <www folder> <log_file> <private_key_file> <cert_file>\n"

/* @ubuntu:sudo ./webos 80 443 www log CA/private/server.key CA/certs/server.crt cgi_folder
 *sudo ./webos 80 443 www log CA/private/server.key CA/certs/server.crt flask/cgi_script.py
 */

char g_www_root_folder[MAX_PATH_NAME];
char g_cgi_folder[MAX_PATH_NAME];

int
main(int argc, char **argv)
{
	char *log_file;
	//char *lock_file;

	web_engine_t engine;
	
	if (argc < 8)
	{
		fprintf(stdout, USAGE, argv[0]);
		return -1;
	}
	memset(&engine, 0, sizeof(engine));
	engine.http_port = atoi(argv[1]);
	engine.https_port = atoi(argv[2]);
	strcpy(g_www_root_folder, argv[3]);
	printf("www: %s\n", g_www_root_folder);
	log_file = argv[4];
	//lock_file = argv[3];
	engine.key = argv[5];
	engine.cert = argv[6];
	strcpy(g_cgi_folder, argv[7]);
	log_init(log_file);
	web_engine_creat(&engine);
	
	web_log(WEB_LOG_EVENT, "Web server starting on port %d\n", engine.http_port);

	web_engine_event_loop(&engine);
	return 0;
}

