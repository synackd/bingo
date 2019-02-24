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
#include "constants.hpp"

/**
 * Main runtime of manager application.
 */
int main(int argc, char **argv)
{
    /*****************
     * HOUSECLEANING *
     *****************/
     srand(time(NULL)); // Randomize

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

    int newGameID = 1;      // RANDOMIZATION PENDING!!!!!!!!!!

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
                    info("Received Caller: %s\t %d", data.clr_cmd_startgame.callerIP, data.clr_cmd_startgame.callerPort);

                    // Checking if there are enought registered players:
                    if (requestedK <= mgr->numberOfRegPlayers){
                        mgr->sendKPlayers(mgr_sock, data);

                        // Saving Caller Details:
                        Player *inputCaller = new Player("caller", data.clr_cmd_startgame.callerIP, data.clr_cmd_startgame.callerPort);
                        Game *newGame = new Game(newGameID, requestedK, inputCaller);

                        // UPdating gameList and number of games:
                        mgr->gameList.push_back(*newGame);
                        mgr->numberOfGames++;

                    } else {
                        error("There are not enough registered players.");
                        msg_t startGameFail;
                        startGameFail.command = FAILURE;
                        mgr_sock->send((void*) &startGameFail, sizeof(msg_t));
                    }

                    break;

                case QUERY_PLAYERS:
                    info("Number of registered players: %d", mgr->numberOfRegPlayers);
                    mgr->sendAllPlayers(mgr_sock);

                    break;

                // Anything else
                default:
                    error("Unknown command received.");
                    break;
            }
        } else {
            // Restart socket
            info("Restarting socket");
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

bool Manager::checkUsedIDs(int newID, vector<Game> list, int size){
    for (int i = 0; i < size; i ++){
        if (newID == list[i].getID())
            return true;
    }
    return false;
}

int Manager::generateGameID(){
    // Assuming gameIDs between 0-10:
    int newID = rand() & 10;
    while (this->checkUsedIDs(newID, this->gameList, this->numberOfGames)){
        newID = rand() & 10;
    }

    return newID;
}


/**
 * Send k players to caller that requested them
 */
void Manager::sendKPlayers(ServerSocket *sock, msg_t data)
{
    int gameID = this->generateGameID();
    int numberOfPlayersToSend = data.clr_cmd_startgame.k;
    msg_t response;

    info("START_GAME command received.");
    info("Sending %d players to caller...", numberOfPlayersToSend);
    for (int i = 0; i < numberOfPlayersToSend; i++){
        // TODO: Randomize game id (have a global counter?)
        response.mgr_rsp_startgame.gameID = gameID;
        strcpy(response.mgr_rsp_startgame.playerName, registeredPlayers[i].getName().c_str());
        strcpy(response.mgr_rsp_startgame.playerIP, registeredPlayers[i].getIP().c_str());
        response.mgr_rsp_startgame.playerPort = registeredPlayers[i].getPort();

        // Send player data back to caller
        info("Sending player \"%s\": GameID = %d\tIP = %s\tPort = %d", response.mgr_rsp_startgame.playerName, response.mgr_rsp_startgame.gameID, response.mgr_rsp_startgame.playerIP, response.mgr_rsp_startgame.playerPort);
        sock->send((void*) &response, sizeof(msg_t));

        // Waiting for ACK
        sock->receive((void*) &data, sizeof(msg_t));
    }
}

/**
 * Send all registered players to caller requesting them
 */
int Manager::sendAllPlayers(ServerSocket *sock)
{
    int numberOfPlayersToSend = this->numberOfRegPlayers;
    msg_t response;

    info("QUERY_PLAYERS command received.");

    // Check if there are any players
    if (numberOfPlayersToSend == 0) {
        info("There are no players!");

        // Populate response
        response.command = FAILURE;

        // Send response
        sock->send((void*) &response, sizeof(msg_t));
    } else {
        info("Sending %d players to caller...", numberOfPlayersToSend);
        for (int i = numberOfPlayersToSend; i > 0; i--) {
            // Populate response
            response.command = SUCCESS;
            response.mgr_rsp_queryplayers.players_left = i - 1;
            strncpy(response.mgr_rsp_queryplayers.name, registeredPlayers[i-1].getName().c_str(), BUFMAX);
            strncpy(response.mgr_rsp_queryplayers.ip, registeredPlayers[i-1].getIP().c_str(), BUFMAX);
            response.mgr_rsp_queryplayers.port = registeredPlayers[i-1].getPort();

            // Send response
            info("Sending player \"%s\": IP: %s\tPort: %d", response.mgr_rsp_queryplayers.name, response.mgr_rsp_queryplayers.ip, response.mgr_rsp_queryplayers.port);
            sock->send((void*) &response, sizeof(msg_t));
        }
    }

    return numberOfPlayersToSend;
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
    for (int i = 0; i < this->registeredPlayers.size(); i++) {
        if (this->registeredPlayers[i].getName() == name) {
            // Player is in list, deregister
            this->registeredPlayers.erase(this->registeredPlayers.begin()+i);
            this->numberOfRegPlayers--;
            return SUCCESS;
        }
    }

    // Otherwise, fail
    return FAILURE;
}
