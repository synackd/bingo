/**
 * @file server.cpp
 *
 * Class definitions for a server socket.
 */

#include "server.hpp"

using namespace std;

/**
 * Create a socket to listen on.
 *
 * @param port Port to bind the socket to.
 */
ServerSocket::ServerSocket(unsigned short port)
{
    /* Create socket */
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // server: socket() failed
        // Delete this
    }

    /* Construct local address structure */
    memset(&this->srvAddr, 0, sizeof(this->srvAddr));       // Reset memory of server IP struct
    this->srvAddr.sin_family      = AF_INET;                // Internet address family
    this->srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);      // Convert unsigned int to IP address
    this->srvAddr.sin_port        = htons(port);            // Convert unsigned short to port

    /* Bind to local address */
    if (bind(this->sockfd, (struct sockaddr*) &this->srvAddr, sizeof(this->srvAddr)) < 0) {
        // server: bind() failed
        // Delete this
    }

    /* Listen for incoming requests */
    if (listen(this->sockfd, BACKLOG) < 0) {
        // server: listen() failed
        // Delete this
    }
}

/**
 * Close the connection and close the socket on delete.
 */
ServerSocket::~ServerSocket(void)
{
    if (this->connfd)
        this->stop();
    if (this->sockfd)
        close(sockfd);
}

/**
 * Accept incoming connections on the socket.
 *
 * @return The connection file descriptor.
 */
int ServerSocket::start(void)
{
    /* Check for valid socket */
    if (this->sockfd == 0) {
        // server: no socket to listen on
        return -1;
    }

    /* Start accepting connections */
    unsigned int cliAddrLen = sizeof(this->cliAddr);
    this->connfd = accept(this->sockfd, (struct sockaddr*) &this->cliAddr, &cliAddrLen);

    /* Return connection file descriptor */
    return this->connfd;
}

/**
 * Close the socket's connection.
 *
 * @return The return code of the close() function.
 */
int ServerSocket::stop(void)
{
    return close(this->connfd);
}

/**
 * Read data from the socket and store it.
 *
 * @param data Where to put the read data. CANNOT BE A NULL
 * POINTER! Set this to a 'any_cmd_t' in order to determine
 * the command code, then cast to the appropriate structure.
 * @param size The number of bytes so receive.
 * @return The size of the data read.
 */
ssize_t ServerSocket::receive(void** data, size_t size)
{
    /* Don't write to a nonexistent or NULL pointer */
    if (data == NULL || *data == NULL)
        return -1;

    /* Read data, store in 'data', and return # bytes read */
    return read(this->sockfd, *data, size);
}

/**
 * Write data to the socket.
 *
 * @param data The data to write to the socket. Will not
 * work with a NULL pointer.
 * @param size The number of bytes to send.
 * @return Status of the write.
 */
ssize_t ServerSocket::send(void* data, size_t size)
{
    /* Don't send NULL data */
    if (data == NULL)
        return -1;

    return write(this->sockfd, data, size);
}
