/**
 * @file socket.hpp
 */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <sys/socket.h> /* for socket() and bind() */
#include <stdio.h>      /* printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

/**
 * An abstract class representing a general-
 * purpose socket.
 */
class Socket
{
    public:
        int getSocket(void);
        int getConnection(void);
        virtual ssize_t receive(void* data, size_t size) = 0;
        virtual ssize_t send(void* data, size_t size) = 0;

    protected:
        int sockfd;     /**< File descriptor of socket */
        int connfd;     /**< File descriptor of connection */
        bool verbose;   /**< Whether to print to stdout */
};

#endif
