#!/bin/sh
#sudo valgrind ./webos 80 4443 www/ log CA/private/server.key CA/certs/server.crt flask/flaskr.py lock_file
sudo ./webos 80 4443 www/ log CA/private/server.key CA/certs/server.crt flask/flaskr.py lock_file
