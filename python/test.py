#!/usr/bin/python

from socket import *
import sys
import random
import os

if len(sys.argv) < 7:
    sys.stderr.write("Usage: %s <ip> <port> <#trials> <#writes and reads per trial> <max # bytes to	write at a time> <#connections>\n" % (sys.argv[0]));
    sys.exit(1);


serverHost = sys.argv[1]
serverPort = int(sys.argv[2])
numTrials = int(sys.argv[3])
numWritesReads = int(sys.argv[4])
numBytes = int(sys.argv[5])
numConnections = int(sys.argv[6])

socketList = []
buf = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

for i in xrange(numConnections):
    s = socket(AF_INET, SOCK_STREAM)
    s.connect((serverHost, serverPort))
    socketList.append(s)

for i in xrange(numTrials):

    for j in xrange(numWritesReads):
        print j
	print "send:" + buf
	socketList[j].send(buf)
		
    print "----------------------------------"
    print "Begin to read"
    raw_input("Input: ")
    for j in xrange(numWritesReads):
	data = socketList[j].recv(len(buf))
	if (data != buf):
            sys.stderr.write("Error: data received is not the same ad sent! recv: (%d, %s)" %(j, data))
	    sys.exit(1)
        else:
            print "read correctly"

for i in xrange(numConnections):
    socketList[i].close()

print "Test successfully!"
