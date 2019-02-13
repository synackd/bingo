/**
 * @file client.hpp
 *
 * Defines a client socket.
 */

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "socket.hpp"

class ClientSocket: public Socket
{
    private:
        struct sockaddr_in srvAddr;     /**< The client's IP address */

    public:
        ClientSocket(string ip, unsigned short port);
        ~ClientSocket(void);
        int start(void);
        int stop(void);
        ssize_t send(void* data, size_t size);
        ssize_t receive(void** data, size_t size);
};

#endif
