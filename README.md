#webos: A Little HTTP/1.1 Web Server
##File Description
src/: all c source files;
inlcude/: all header files;
test/: test fils;
flask/: cgi files;
CA/: certificate used for https;
www/: server hosted static files;
objs/: object files;

##Usage:	
	./webos [http_port] [https_port] [www_root_folder] [log_file] [server_key] [server_cert]
	[cgi_script] [lock_file]	
	eg: ./webos 80 443 www log CA/private/server.key CA/certs/server.crt flask/flaskr.py lock_file
