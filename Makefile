#
# Makefile for webos
#
# Jin <jinyang.hust@gmail.com>
#

define build-cmd
$(CC) $(CFLAGS) $< -o $@
endef

CC = gcc
CFLAGS = -g -Wall -Werror -O2 
CLIB = -lssl -lcrypto
SOURCE = src
VPATH = $(SOURCE)
OBJECTS = webos.o
#OBJECTS += web_engine.o
OBJECTS += connection.o
#OBJECTS += http.o
OBJECTS += os.o
OBJECTS += log.o
OBJECTS += slist.o
OBJECTS += hash.o
OBJECTS += util.o
OBJECTS += ssl.o
OBJECTS += cgi.o
OBJECTS += daemon.o
OBJECTS += listen.o
OBJECTS += event.o
OBJECTS += select.o
OBJECTS += core.o


default: webos

webos: $(OBJECTS)
	$(CC) $(CFLAGS) -o webos $(OBJECTS) $(CLIB)

$(SOURCE)/%.o: common.h list.h
	$(build-cmd)

event.o: select.o

.PHONY: clean

clean:
	-rm -f webos
	-rm ./*.o
