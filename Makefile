CC=gcc
CFLAGS=-std=c++11

client: client
     $(CC) $(CFLAGS) -o client client.cpp

server: server
	 $(CC) $(CFLAGS) -o server server.cpp
