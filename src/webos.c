/*
 * =====================================================================================
 *
 *       Filename:  webos.c
 *
 *    Description:  main web server exec routine
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *			Usage:	./webos <http_port> <https_port> <www folder> <log_file> <private_key_file> <cert_file> <cgi_folder> <lock_file>\n"

 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
//#include "core.h"
//#include "http.h"
#include "core.h"
#include "log.h"
#include <string.h>
#include "daemon.h"

#define USAGE	"\nUsage: %s <http_port> <https_port> <www folder> <log_file> <private_key_file> <cert_file> <cgi_folder> <lock_file>\n"

/* @ubuntu:sudo ./webos 80 443 www log CA/private/server.key CA/certs/server.crt cgi_folder
 * eg: sudo ./webos 80 443 www log CA/private/server.key CA/certs/server.crt
 * flask/cgi_script.py lock_file
 */
#define MAX_PATH_NAME   128

char g_www_root_folder[MAX_PATH_NAME];
char g_cgi_folder[MAX_PATH_NAME];

int
main(int argc, char **argv)
{
	char *log_file;
	//char *lock_file;
    int http_port, https_port;

	web_engine_t engine;
	
	if (argc < 8)
	{
		fprintf(stdout, USAGE, argv[0]);
		return -1;
	}
	memset(&engine, 0, sizeof(engine));
	http_port = atoi(argv[1]);
	https_port = atoi(argv[2]);
	strcpy(g_www_root_folder, argv[3]);
	log_file = argv[4];
	engine.key = argv[5];
	engine.cert = argv[6];
	strcpy(g_cgi_folder, argv[7]);
	//lock_file = argv[8];

    /* initialize modules */
	log_init(log_file);
    event_core_init();
#if 0
	if (daemonize(lock_file) < 0)
	{
		fprintf(stderr, "open lock_file failed\n");
		return -1;
	}
#endif
    printf("port1=%d, port2=%d\n", http_port, https_port);
	//web_engine_creat(&engine);
	core_engine_init(&engine, http_port, https_port);
	web_log(WEB_LOG_EVENT, "Web server starting on port %d\n", http_port);

    printf("port1111=%d, port2222=%d\n", http_port, https_port);
	//web_engine_event_loop(&engine);
    core_engine_run();
	return 0;
}

