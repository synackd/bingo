/**
 * @file client.cpp
 *
 * Class definitions for a client socket.
 */

#include <string.h>
#include <errno.h>
#include "constants.hpp"
#include "colors.hpp"
#include "client.hpp"

using namespace std;

/**
 * Create a socket to listen on.
 *
 * @param port Port to bind the socket to.
 */
ClientSocket::ClientSocket(string ip, unsigned short port)
{
    /* Create socket */
    errno = 0;
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Creating socket...\n");
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        return;
    }
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Socket created.\n");

    /* Construct local address structure */
    memset(&this->srvAddr, 0, sizeof(this->srvAddr));           // Reset memory of client IP struct
    this->srvAddr.sin_family = AF_INET;                         // Internet address family
    this->srvAddr.sin_port   = htons(port);                     // Convert unsigned short to port
    inet_pton(AF_INET, ip.c_str(), &this->srvAddr.sin_addr);    // Convert IP from string to binary
}

/**
 * Close the socket on delete.
 */
ClientSocket::~ClientSocket(void)
{
    if (this->sockfd)
        close(sockfd);
}

/**
 * Accept incoming connections on the socket.
 *
 * @return The status of connect().
 */
int ClientSocket::start(void)
{
    /* Check for valid socket */
    if (this->sockfd == 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "No socket to listen on!\n");
        return -1;
    }

    /* Connect to server */
    errno = 0;
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Connecting to server...\n");
    int status = connect(this->sockfd, (struct sockaddr*) &this->srvAddr, sizeof(this->srvAddr));
    if (status < 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "connect() failed: %s\n", strerror(errno));
        errno = CANNOTCONN;
    }

    return status;
}

/**
 * Close the socket.
 *
 * @return The return code of the close() function.
 */
int ClientSocket::stop(void)
{
    return close(this->sockfd);
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
ssize_t ClientSocket::receive(void* data, size_t size)
{
    /* Make sure there's a socket */
    if (this->sockfd == 0) {
        cprintf(stderr, BOLD, "[SRV][ERR] ");
        fprintf(stderr, "No open connection!\n");
        return -1;
    }

    /* Don't write to a nonexistent or NULL pointer */
    if (data == NULL) {
        cprintf(stderr, BOLD, "[SRC][ERR] ");
        fprintf(stderr, "Received data is NULL!\n");
        return -1;
    }

    /* Read data, store in 'data', and return # bytes read */
    errno = 0;
    ssize_t bytes = read(this->sockfd, data, size);
    if (bytes < 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "read() failed: %s\n", strerror(errno));
        errno = CANNOTREAD;
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
ssize_t ClientSocket::send(void* data, size_t size)
{
    /* Make sure there's a connection */
    if (this->sockfd == 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "No open connection!\n");
        return -1;
    }

    /* Don't send NULL data */
    if (data == NULL) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "Tried to send NULL data.\n");
        return -1;
    }

    /* Write data to socket */
    errno = 0;
    ssize_t bytes = write(this->sockfd, data, size);
    if (bytes < 0) {
        cprintf(stderr, BOLD, "[CLI][ERR] ");
        fprintf(stderr, "write() failed: %s\n", strerror(errno));
        errno = CANNOTWRITE;
    }

    return bytes;
}
