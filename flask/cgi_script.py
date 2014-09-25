#!/usr/bin/env python
#
#
from os import environ
import cgi
import cgitb

cgitb.enable()

print 'HTTP/1.1 200 OK\r\n',
print 'Server: %s\r\n' % (environ['SERVER_SOFTWARE']),

cgi.test()
