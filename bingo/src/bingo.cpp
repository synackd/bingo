/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include "colors.hpp"
#include "bingo.hpp"
#include "cmd.hpp"
#include "client.hpp"

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
    // Arg checking.
    if (argc != 3) {
        cprintf(stdout, BOLD, "Usage: ");
        fprintf(stdout, "%s <server_ip> <port>\n", argv[0]);
        exit(FAILURE);
    }

    ssize_t size = 0;

    // Create general struct.
    any_cmd_t general = { .command = REGISTER };    // For sending
    any_cmd_t *rec    = (any_cmd_t*) malloc(sizeof(any_cmd_t)); // Receiving

    // Create socket and start it.
    ClientSocket *sock = new ClientSocket(argv[1], 4000);
    sock->start();

    // Send struct through socket and read response.
    size = sock->send(&general, sizeof(general));
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Sent %d bytes over socket.\n", size);
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Listening for response...\n");

    size = sock->receive((void**) &rec, sizeof(*rec));
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Received %d bytes over socket.\n", size);
    cprintf(stdout, BOLD, "[CLI] ");
    fprintf(stdout, "Command: %d\n", rec->command);

    // Clean up.
    sock->stop();
    free(rec);
    delete sock;

    return 0;
}
