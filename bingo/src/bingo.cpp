/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "colors.hpp"
#include "bingo.hpp"
#include "cmd.hpp"
#include "client.hpp"

/**
 * Log an info message to stdout
 * from the manager.
 */
void info(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stdout, BOLD, "[PLR] ");
    va_start(vaList, fmt);
    vfprintf(stdout, fmt, vaList);
    va_end(vaList);
    fprintf(stdout, "\n");
}

/**
 * Log an error message to stderr
 * from the manager.
 */
void error(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stderr, BOLD, "[PLR][ERR] ");
    va_start(vaList, fmt);
    vfprintf(stderr, fmt, vaList);
    va_end(vaList);
    fprintf(stderr, "\n");
}

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

    // Try to convert port.
    unsigned short port;
    errno = 0;
    long int tmp = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        error("Invalid port number.");
        exit(FAILURE);
    }
    if (0 <= tmp && tmp <= 65535) {
        port = (unsigned short) tmp;
    } else {
        error("Port must be between 0 and 65535.");
        exit(FAILURE);
    }

    ssize_t size = 0;

    // Create general payload.
    msg_t general;
    general.command = REGISTER;
    strncpy(general.mgr_cmd_register.name, "Player", BUFMAX);
    strncpy(general.mgr_cmd_register.ip, "1.1.1.1", BUFMAX);
    strncpy(general.mgr_cmd_register.port, "4000", BUFMAX);

    msg_t rec; // Receiving

    // Create socket and start it.
    ClientSocket *sock = new ClientSocket(argv[1], port);
    sock->start();

    // Send struct through socket and read response.
    size = sock->send(&general, sizeof(msg_t));
    info("Sent %d bytes over socket.", size);
    info("Listening for response...");

    size = sock->receive((void*) &rec, sizeof(msg_t));
    info("Received %d bytes over socket.", size);
    info("Command Return: %d", rec.mgr_rsp_register.ret_code);
    info("Game ID: %d", rec.mgr_rsp_register.game_uid);

    // Clean up.
    sock->stop();
    delete sock;

    return 0;
}
