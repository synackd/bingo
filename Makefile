CC=gcc
CFLAGS=

client: client
     $(CC) -o client client.cpp

server: server
	 $(CC) -o server server.cpp
