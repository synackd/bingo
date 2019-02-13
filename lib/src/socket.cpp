/**
 * @brief Definitions for a general socket.
 */

#include "socket.hpp"

using namespace std;

/**
 * Return a file descriptor of the socket.
 */
Socket::getSocket()
{
    return this.sockfd;
}

/**
 * Return a file descriptor of the connection.
 */
Socket::getConnection()
{
    return this.connfd;
}
