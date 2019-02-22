/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "colors.hpp"
#include "log.hpp"
#include "manager.hpp"
#include "player.hpp"
#include "cmd.hpp"
#include "server.hpp"

/**
 * Main runtime of manager application.
 */
int main(int argc, char **argv)
{
    /*****************
     * HOUSECLEANING *
     *****************/

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

    /******************************
     * COMMUNICATING WITH CALLERS *
     ******************************/

    // Create new manager object
    Manager *mgr = new Manager();

    // Create socket to listen for client
    ServerSocket *mgr_sock = new ServerSocket(port);
    mgr_sock->start();

    // Continuously listen for requests and decide what to
    // do based on data
    int status;
    ssize_t size = 0;
    msg_t data;
    for ( ; ; ) {
        if ((size = mgr_sock->receive((void*) &data, sizeof(msg_t))) != 0) {
            info("Received %d bytes from socket.", size);

            /*
             * Determine what to do based on command
             *
             * NOTE: Responses have the "command" field in msg_t set with
             * the return code of the request. Return codes are 0 or less,
             * distinguishing them from commands.
             */
            switch (data.command) {
                // Register command
                case REGISTER:
                    // Register the player
                    info("Command received was REGISTER.");
                    info("Attempting to register player \"%s\" with IP \"%s\" and port %d...", data.mgr_cmd_register.name, data.mgr_cmd_register.ip, data.mgr_cmd_register.port);
                    status = mgr->registerPlayer(data.mgr_cmd_register.name, data.mgr_cmd_register.ip, data.mgr_cmd_register.port);

                    // Form response
                    msg_t mgr_rsp;
                    if (status == SUCCESS) {
                        info("Registration succeeded!");
                        mgr_rsp.command = SUCCESS;
                        mgr_rsp.mgr_rsp_register.ret_code = SUCCESS;
                    } else {
                        error("Registration failed!");
                        mgr_rsp.command = FAILURE;
                        mgr_rsp.mgr_rsp_register.ret_code = FAILURE;
                    }

                    // Send response
                    size = mgr_sock->send((void*) &mgr_rsp, sizeof(msg_t));
                    info("Sent response of %d bytes.", size);

                    break;
                // Anything else
                default:
                    error("Unknown command received.");
                    break;
            }
        } else {
            // Restart socket
            mgr_sock->stop();
            mgr_sock->start();
        }
    }
}

/*
 * Class Implementations
 */

/***********
 * Manager *
 ***********/

/**
 * Create a new manager
 */
Manager::Manager()
{
}

/**
 * Send k players to caller that requested them
 */
void Manager::sendKPlayers()
{
}

/**
 * Register players who want to be considered to play Bingo
 */
int Manager::registerPlayer(string name, string ip, unsigned int port)
{
    // Check that player isn't already in list
    for (int i = 0; i < registeredPlayers.size(); i++) {
        if (registeredPlayers[i].getName() == name)
            return FAILURE;
    }

    // If not, add player
    PlayerData newPlayer(name, ip, port);
    registeredPlayers.push_back(newPlayer);

    return SUCCESS;
}
