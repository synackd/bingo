/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include "colors.hpp"
#include "manager.hpp"
#include "cmd.hpp"
#include "server.hpp"

/**
 * Main runtime of manager application.
 */
int main(int argc, char **argv)
{
    // Arg checking.
    if (argc != 2) {
        cprintf(stdout, BOLD, "Usage: ");
        fprintf(stdout, "%s <port>\n", argv[0]);
        exit(FAILURE);
    }

    // Try to convert port.
    unsigned short port;
    errno = 0;
    long int tmp = strtol(argv[1], NULL, 10);
    if (errno != 0) {
        cprintf(stderr, BOLD, "Error: ");
        fprintf(stderr, "Invalid port number.\n");
        exit(FAILURE);
    }
    if (0 <= tmp && tmp <= 65535) {
        port = (unsigned short) tmp;
    } else {
        cprintf(stderr, BOLD, "Error: ");
        fprintf(stderr, "Port must be between 0 and 65535.\n");
        exit(FAILURE);
    }

    ssize_t size = 0;

    // Create a general struct.
    any_cmd_t general = { .command = DEREGISTER };     // For sending
    any_cmd_t *rec    = (any_cmd_t*) malloc(sizeof(any_cmd_t)); // Receiving

    // Create socket and start it.
    ServerSocket *sock = new ServerSocket(port);
    sock->start();

    // Receive data from client and send response.
    size = sock->receive((void**) &rec, sizeof(*rec));
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Received %d bytes over socket.\n", size);
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Command: %d\n", rec->command);

    size = sock->send(&general, sizeof(general));
    cprintf(stdout, BOLD, "[SRV] ");
    fprintf(stdout, "Sent %d bytes over socket.\n", size);

    return 0;
}
