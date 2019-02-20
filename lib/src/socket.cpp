/**
 * @brief Definitions for a general socket.
 */

#include "socket.hpp"

using namespace std;

/**
 * Return a file descriptor of the socket.
 */
int Socket::getSocket()
{
    return this->sockfd;
}

/**
 * Return a file descriptor of the connection.
 */
int Socket::getConnection()
{
    return this->connfd;
}
