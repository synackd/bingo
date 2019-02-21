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
#include "manager.hpp"
#include "player.hpp"
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

void startGame(ServerSocket *socket){

}


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
    while ((size = mgr_sock->receive((void*) &data, sizeof(msg_t))) != 0) {
        info("Received %d bytes from socket.", size);

        // Determine what to do
        if (data.command == REGISTER) {
            // Register the player
            info("Command received was REGISTER.");
            info("Attempting to register player \"%s\"...", data.mgr_cmd_register.name);
            status = mgr->registerPlayer(data.mgr_cmd_register.name, data.mgr_cmd_register.ip, data.mgr_cmd_register.port);

            // Form response
            msg_t mgr_rsp;
            if (status == SUCCESS) {
                info("Registration succeeded!");
                mgr_rsp.mgr_rsp_register.ret_code = SUCCESS;
            } else {
                error("Registration failed!");
                mgr_rsp.mgr_rsp_register.ret_code = FAILURE;
            }

            // Send response
            size = mgr_sock->send((void*) &mgr_rsp, sizeof(msg_t));
            info("Sent response of %d bytes.", size);
        }else if (data.command == START_GAME) {
            info("Command received was START_GAME.");

        }else{
            error("Unknown command received.");
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
