# Makefile for Chat Server and Client

CC = gcc
CFLAGS = -Wall -pthread -g
TARGETS = chatserver chatclient

all: $(TARGETS)

chatserver: server/server.c
	$(CC) $(CFLAGS) -o chatserver server/server.c

chatclient: client/client.c
	$(CC) $(CFLAGS) -o chatclient client/client.c

clean:
	rm -f $(TARGETS) server.log

run-server: chatserver
	./chatserver 5000

run-client: chatclient
	./chatclient 127.0.0.1 5000

.PHONY: all clean run-server run-client