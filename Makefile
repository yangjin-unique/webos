#
# Makefile for webos
#
# Jin <jinyang.hust@gmail.com>
#

CC = gcc
CFLAGS = -g -Wall -Werror -O2 -Iinclude
CLIB = -lssl -lcrypto

SOURCES = $(wildcard src/*.c)
NO_DIR_SRC = $(notdir $(SOURCES))
OBJECTS = $(patsubst %.c, objs/%.o, $(NO_DIR_SRC))

TARGET = webos


all: build $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(CLIB)

objs/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

listen.o: connection.o

build:
	@mkdir -p objs

.PHONY: clean

clean:
	-rm -f webos
	-rm -rf objs/
