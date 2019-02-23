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

    // Create new manager object
    Manager *mgr = new Manager();

    // Create socket to listen for client
    ServerSocket *mgr_sock = new ServerSocket(port);
    mgr_sock->start();

    // Continuously listen for requests and decide what to
    // do based on data
    int status, requestedK;
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
            msg_t mgr_rsp;
            switch (data.command) {
                // Register command
                case REGISTER:
                    // Register the player
                    info("Command received was REGISTER.");
                    info("Attempting to register player \"%s\" with IP \"%s\" and port %d...", data.mgr_cmd_register.name, data.mgr_cmd_register.ip, data.mgr_cmd_register.port);
                    status = mgr->registerPlayer(data.mgr_cmd_register.name, data.mgr_cmd_register.ip, data.mgr_cmd_register.port);

                    // Form response
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

                case DEREGISTER:
                    // Deregister the player
                    info("Command received was DEREGISTER.");
                    info("Attempting to deregister player \"%s\"...", data.mgr_cmd_deregister.name);
                    status = mgr->deregisterPlayer(data.mgr_cmd_deregister.name);

                    // Form response
                    if (status == SUCCESS) {
                        info("Deregistration succeeded!");
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

                case START_GAME:
                    info("Registered Players Count: %d", mgr->numberOfRegPlayers);
                    requestedK = data.clr_cmd_startgame.k;

                    if (requestedK <= mgr->numberOfRegPlayers){
                        mgr->sendKPlayers(mgr_sock, data);
                    } else {
                        error("There are not enough registered players.");
                        msg_t startGameFail;
                        startGameFail.command = FAILURE;
                        mgr_sock->send((void*) &startGameFail, sizeof(msg_t));
                    }

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
    this->numberOfRegPlayers = 0;
}

/**
 * Send k players to caller that requested them
 */
void Manager::sendKPlayers(ServerSocket *sock, msg_t data)
{
    int numberOfPlayersToSend = data.clr_cmd_startgame.k;
    msg_t response;

    info("START_GAME command received.");
    info("Sending %d players to caller...", numberOfPlayersToSend);
    for (int i = 0; i < numberOfPlayersToSend; i++){
        // TODO: Randomize game id (have a global counter?)
        response.mgr_rsp_startgame.gameID = 1;
        strcpy(response.mgr_rsp_startgame.playerName, registeredPlayers[i].getName().c_str());
        strcpy(response.mgr_rsp_startgame.playerIP, registeredPlayers[i].getIP().c_str());
        response.mgr_rsp_startgame.playerPort = registeredPlayers[i].getPort();

        // Send player data back to caller
        info("Sending Player: GameID = %d\tIP = %s\tPort = %d", response.mgr_rsp_startgame.gameID, response.mgr_rsp_startgame.playerIP, response.mgr_rsp_startgame.playerPort);
        sock->send((void*) &response, sizeof(msg_t));

        // Waiting for ACK
        sock->receive((void*) &data, sizeof(msg_t));
        if (data.command == CALLERACK)
            info("Caller ACK received.");
    }
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
    numberOfRegPlayers++;

    return SUCCESS;
}

/**
 * Deregister a player
 */
int Manager::deregisterPlayer(string name)
{
    // Check if player is in list
    for (int i = 0; i < registeredPlayers.size(); i++) {
        if (registeredPlayers[i].getName() == name) {
            // Player is in list, deregister
            registeredPlayers.erase(registeredPlayers.begin()+i);
            return SUCCESS;
        }
    }

    // Otherwise, fail
    return FAILURE;
}
