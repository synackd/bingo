/**
 * @file server.cpp
 *
 * Class definitions for a server socket.
 */

#include <errno.h>
#include <string.h>
#include "colors.hpp"
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
    errno = 0;
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Creating socket...\n");
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        return;
    }
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Socket created.\n");

    /* Construct local address structure */
    memset(&this->srvAddr, 0, sizeof(this->srvAddr));       // Reset memory of server IP struct
    this->srvAddr.sin_family      = AF_INET;                // Internet address family
    this->srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);      // Convert unsigned int to IP address
    this->srvAddr.sin_port        = htons(port);            // Convert unsigned short to port

    /* Bind to local address */
    errno = 0;
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Binding socket to port %d...\n", port);
    if (bind(this->sockfd, (struct sockaddr*) &this->srvAddr, sizeof(this->srvAddr)) < 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "bind() failed: %s\n", strerror(errno));
        return;
    }
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Socket bound.\n");

    /* Listen for incoming requests */
    errno = 0;
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Listening on port %d...\n", port);
    if (listen(this->sockfd, BACKLOG) < 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "listen() failed: %s\n", strerror(errno));
        return;
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
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "No socket to listen on!\n");
        return -1;
    }

    /* Start accepting connections */
    this->cliAddrLen = sizeof(this->cliAddr);
    this->connfd = accept(this->sockfd, (struct sockaddr*) &this->cliAddr, &cliAddrLen);
    if (this->connfd < 0) {
        cprintf(stdout, BOLD, "[SRV] ");
        fprintf(stdout, "accept() failed: %s\n", strerror(errno));
    }

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
    /* Make sure there's a connection */
    if (this->connfd == 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "No open connection!\n");
        return -1;
    }

    /* Don't write to a nonexistent or NULL pointer */
    if (data == NULL || *data == NULL) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "Received data is NULL!\n");
        return -1;
    }

    /* Read data, store in 'data', and return # bytes read */
    errno = 0;
    ssize_t bytes = read(this->connfd, *data, size);
    if (bytes < 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "read() failed: %s\n", strerror(errno));
    }

    return bytes;
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
    /* Make sure there's a connection */
    if (this->connfd == 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "No open connection!\n");
        return -1;
    }

    /* Don't send NULL data */
    if (data == NULL) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "Tried to send NULL data.\n");
        return -1;
    }

    /* Write data to socket */
    errno = 0;
    ssize_t bytes = write(this->connfd, data, size);
    if (bytes < 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "write() failed: %s\n", strerror(errno));
    }

    return bytes;
}
