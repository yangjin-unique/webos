webos: a little http/1.1 web server

Usage:	
	./webos [http_port] [https_port] [www_root_folder] [log_file] [server_key] [server_cert]
	[cgi_script] [lock_file]	
	eg: ./webos 80 443 www log CA/private/server.key CA/certs/server.crt flask/flaskr.py lock_file
