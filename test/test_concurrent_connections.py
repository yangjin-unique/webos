#!/usr/bin/python

from socket import *
import sys
import random
import os
import time


serverHost = "127.0.0.1"
serverPort = 80
numConnections = [10, 100, 500, 1000, 2000, 10000]
numTrials = len(numConnections)

buf = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" #send content

def timeit(method):
    def timed(*args, **kw):
        ts = time.time()
        result = method(*args, **kw)
        te = time.time()
        print "Total time:  %2.2f sec" %(te-ts)
        return result
    return timed


@timeit
def test(num):
    socketList = []
    for i in xrange(num):
        s = socket(AF_INET, SOCK_STREAM)
        s.connect((serverHost, serverPort))
        socketList.append(s)

    # send buf for each connection
    for j in xrange(num):
        #print "send:" + buf
        socketList[j].send(buf)
                
    #print "Begin to read"
    #raw_input("Input: ")
    for j in xrange(num):
        data = socketList[j].recv(len(buf))
        if (data != buf):
            #sys.stderr.write("Error: data received is not the same ad sent! recv: (trial=%d, index=%d, %s)\n" %(i, j, data))
            print "-----Failed------"
            sys.exit(1)

    for i in xrange(num):
        socketList[i].close()

    print "++++++Successfully++++++" 


if __name__ == "__main__":
    for k in xrange(numTrials):
        print "Test Case %d: %d concurrnet connections:" %(k+1, numConnections[k])
        test(numConnections[k])
