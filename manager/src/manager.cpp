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

// GLOBAL LIST OF REGISTERED PLAYERS:
vector<PlayerData> playersList;

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

    // // TEMPORAL LIST OF PLAYERS FOR TESTING: //////////////////////
    //
    // // LIST OF REGISTERED PLAYERS:
    //
    // int numberOfRegPlayers = 5;
    //
    // for (int i = 0; i < numberOfRegPlayers; i++){
    //     string playerIP = "IP" + std::to_string(i);
    //     PlayerData tempPlayer("name", playerIP, i);
    //     playersList.push_back(tempPlayer);
    // }

    // Create new manager object
    Manager *mgr = new Manager();

    // Create socket to listen for client
    ServerSocket *mgr_sock = new ServerSocket(port);
    mgr_sock->start();

    // Continuously listen for requests and decide what to
    // do based on data
    int status;
    ssize_t size = 0;
    int requestedK;
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
                case START_GAME:
                    cout << "Registered Players Count = " << mgr->numberOfRegPlayers << "\n";
                    requestedK = data.clr_cmd_startgame.k;

                    if (requestedK <= mgr->numberOfRegPlayers){
                        mgr->sendKPlayers(mgr_sock, data);
                    }else{
                        cout << "There are not enough registered players.\n";
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

  } // end of while

} // end of main()



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
    numberOfRegPlayers = 0;
}

/**
 * Send k players to caller that requested them
 */
void Manager::sendKPlayers(ServerSocket *sock, msg_t data)
{
    info("Command received was START_GAME.");
    cout << "Getting K ...\n";
    int numberOfPlayersToSend = data.clr_cmd_startgame.k;
    msg_t response;
    // ssize_t size = 0;
    // response.command =

    for (int i = 0; i < numberOfPlayersToSend; i++){
        cout << "Populating response gameID...\n";
        // Player tempPlayer(playersList.at(i).IP, playersList.at(i).Port);
        response.mgr_rsp_startgame.gameID = 1;
        cout << "Populating response playerIP...\n";
        strcpy(response.mgr_rsp_startgame.playerIP, registeredPlayers[i].getIP().c_str());
        cout << "Populating response playerPort...\n";
        response.mgr_rsp_startgame.playerPort = registeredPlayers[i].getPort();

        cout << "Sending Player: GameID = " << response.mgr_rsp_startgame.gameID << "\tIP = " << response.mgr_rsp_startgame.playerIP << "\tPort = " << response.mgr_rsp_startgame.playerPort << "\n";
        // response.gamePlayer->PrintPlayer();
        sock->send((void*) &response, sizeof(msg_t));

        // waiting for ACK:
        sock->receive((void*) &data, sizeof(msg_t));
        if (data.command == CALLERACK)
            printf("Caller ACK received.\n");
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
    numberOfRegPlayers ++;
    return SUCCESS;
}
