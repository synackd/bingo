/**
 * @file server.hpp
 *
 * Defines a server socket.
 */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "socket.hpp"

#define BACKLOG 128     /**< Maximum length of socket queue */

class ServerSocket: public Socket
{
    private:
        struct sockaddr_in srvAddr;     /**< The server's IP address */
        struct sockaddr_in cliAddr;     /**< The client's IP address */
        socklen_t cliAddrLen;           /**< The length of the client's IP address */

    public:
        ServerSocket(unsigned short port);
        ~ServerSocket(void);
        int start(void);
        int stop(void);
        ssize_t send(void* data, size_t size);
        ssize_t receive(void* data, size_t size);
};

#endif
