#
# Makefile for webos
#
# Jin <jinyang.hust@gmail.com>
#

CC = gcc
CFLAGS = -g -Wall -Werror -O2
SOURCE = src
VPATH = $(SOURCE)
OBJECTS = webos.o
OBJECTS += web_engine.o
OBJECTS += connection.o
OBJECTS += http.o
OBJECTS += log.o
OBJECTS += slist.o
OBJECTS += util.o

default: webos

webos: $(OBJECTS)
	$(CC) $(CFLAGS) -o webos $(OBJECTS)

$(SOURCE)/%.o: %.c
	$(build-cmd)

clean:
	rm -f webos
	rm ./*.o
