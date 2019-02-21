/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include <stdarg.h>
#include "colors.hpp"
#include "manager.hpp"
#include "cmd.hpp"
#include "server.hpp"

/**
 * Log an info message to stdout
 * from the manager.
 */
void info(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stdout, BOLD, "[MGR] ");
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
    cprintf(stderr, BOLD, "[MGR][ERR] ");
    va_start(vaList, fmt);
    vfprintf(stderr, fmt, vaList);
    va_end(vaList);
    fprintf(stderr, "\n");
}

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
        error("Invalid port number!");
        exit(FAILURE);
    }
    if (0 <= tmp && tmp <= 65535) {
        port = (unsigned short) tmp;
    } else {
        error("Port must be between 0 and 65535!");
        exit(FAILURE);
    }

    ssize_t size = 0;

    // Create a general data payload.
    msg_t general;
    general.command = DEREGISTER;
    general.mgr_rsp_register.ret_code = 0;
    general.mgr_rsp_register.game_uid = 1234;

    msg_t rec; // Receiving

    // Create socket and start it.
    ServerSocket *sock = new ServerSocket(port);
    sock->start();

    // Receive data from client and send response.
    size = sock->receive((void*) &rec, sizeof(msg_t));
    info("Received %d bytes over socket.", size);
    info("Name: %s", rec.mgr_cmd_register.name);

    size = sock->send(&general, sizeof(msg_t));
    info("Sent %d bytes over socket.", size);

    return 0;
}
