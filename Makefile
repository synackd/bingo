CC=gcc
CFLAGS=-std=c++11

client: client
     $(CC) -o client client.cpp

server: server
	 $(CC) -o server server.cpp
