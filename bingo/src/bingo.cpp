/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <vector>
#include <thread>
#include "colors.hpp"
#include "log.hpp"
#include "bingo.hpp"
#include "cmd.hpp"
#include "constants.hpp"
#include "client.hpp"
#include "server.hpp"
#include "input.hpp"
#include "player.hpp"
#include "peer.hpp"
#include "input.hpp"

using namespace std;

/*
 * Data
 */
ClientSocket *bingo_sock;   /**< Socket for communicating with the manager */

char mgr_ip[BUFMAX];        /**< Holds the manager's IP address */
unsigned int mgr_port;      /**< Holds the manager's port */
Game *myGame;               /**< This peer's game */
Peer *me;                   /**< Holds this peer's player data */


/**
 * Menu system for allowing the peer to decide
 * what to do (e.g. be a caller, deregister, etc.
 */
void printMenu()
{
    // Header
    cprintf(stdout, BOLD, "\n Choices:\n");

    // Exit
    cprintf(stdout, BOLD, "  0) ");
    fprintf(stdout, "Exit\n");

    // Start Game K
    cprintf(stdout, BOLD, "  1) ");
    fprintf(stdout, "Start Game\n");

    // Deregister
    cprintf(stdout, BOLD, "  2) ");
    fprintf(stdout, "Deregister\n");

    // Reregister
    cprintf(stdout, BOLD, "  3) ");
    fprintf(stdout, "Reregister\n");

    // Query Players
    cprintf(stdout, BOLD, "  4) ");
    fprintf(stdout, "Query Players\n");

    fprintf(stdout, "\n");
}

/**
 * Get user's choice after printing menu options
 */
int getChoice()
{
    char *choice_str;
    long int tmp = -1;
    int choice;

    // Get user input
    cprintf(stdout, BOLD, "Your choice: ");
    choice_str = read_line();

    // Convert choice
    errno = 0;
    tmp = strtol(choice_str, NULL, 10);
    while (errno != 0 || tmp == -1) {
        cprintf(stdout, BOLD, "Invalid choice!\n");
        cprintf(stdout, BOLD, "Your choice: ");
        choice_str = read_line();
        errno = 0;
        tmp = strtol(choice_str, NULL, 10);
    }
    choice = (int) tmp;

    return choice;
}

/**
 * Menu system for registering this peer with the
 * manager.
 */
void getPeerInfo(char **name_ptr, char **ip_ptr, unsigned int *port_ptr)
{
    // Necessary pointer housekeeping
    if (!name_ptr || !ip_ptr || !port_ptr) {
        error("Cannot write to NULL!");
        exit(FAILURE);
    }

    char *prt;
    int status;
    struct sockaddr_in addr;

    cprintf(stdout, BOLD, " +-------------------+\n");
    cprintf(stdout, BOLD, " | Welcome to Bingo! |\n");
    cprintf(stdout, BOLD, " +-------------------+\n\n");
    cprintf(stdout, BOLD, "Please register.\n");

    // Name
    cprintf(stdout, BOLD, "Name: ");
    *name_ptr = read_line();

    // IP Address
    status = 0;
    while (status != 1) {
        cprintf(stdout, BOLD, "IP Address: ");
        *ip_ptr = read_line();
        status = inet_pton(AF_INET, *ip_ptr, &addr.sin_addr);
        if (status != 1) {
            cprintf(stdout, BOLD, "Invalid IP address!\n");
        }
    }

    // Port
    cprintf(stdout, BOLD, "Port: ");
    prt = read_line();
    errno = 0;
    long int tmp = strtol(prt, NULL, 10);
    while (errno != 0 || !(0 < tmp && tmp <= 65535)) {
        if (errno != 0) {
            errno = 0;
        } else {
            cprintf(stdout, BOLD, "Port must be between 1 and 65535.\n");
        }
        cprintf(stdout, BOLD, "Port: ");
        prt = read_line();
        tmp = strtol(prt, NULL, 10);
    }
    *port_ptr = (unsigned short) tmp;
}

/**
 * The runtime of the listener thread that is created
 * after the player registers
 */
void listener(Bingo *bng)
{
    // Open log file
    FILE *listen_fp = fopen("listener.txt", "w+");
    if (!listen_fp) {
        error("Could not open listener log file!");
        fclose(listen_fp);
        return;
    }

    // After registration, creating socket for listening for new games:
    log(listen_fp, "Listening on default port %d for starting games...", me->getPort());
    ServerSocket *listener_sock = new ServerSocket(me->getPort(), false);  // For listening on default port
    int status = listener_sock->start();

    // Make sure socket starts
    if (status == FAILURE) {
        log(listen_fp, "Could not start socket!");
        fclose(listen_fp);
        return;
    }

    unsigned int remote_callerGamePort;

    // Close log file to write everything
    fclose(listen_fp);
    listen_fp = 0;

    // Listen for invitations to other games
    ssize_t size;
    msg_t data, handshakeResponse;
    while (true) {
        if ((size = listener_sock->receive((void*) &data, sizeof(msg_t))) != 0) {
            switch (data.command) {
                // Negotiate new port for game communication
                case PORT_HANDSHAKE:
                    // Reopen log file for writing
                    listen_fp = fopen("listener.txt", "w+");
                    if (!listen_fp) {
                        error("Could not open listener log file!");
                        fclose(listen_fp);
                        return;
                    }

                    // Create a new player
                    Player *new_player = new Player(me->getName(), me->getIP(), me->getPort());

                    // Extract caller's port
                    unsigned int remote_callerGamePort = data.port_handshake.gamePort;
                    log(listen_fp, "Caller's game port received: %d", remote_callerGamePort);

                    // Find the next available port
                    log(listen_fp, "Finding next available port...");
                    ServerSocket *game_sock;                // Socket for testing availability
                    unsigned int next_port = me->getPort(); // Current default port
                    do {
                        // Reset socket if exists
                        if (game_sock) {
                            delete game_sock;
                            game_sock = NULL;
                        }

                        // Test the next port
                        next_port = (next_port + 1) % 65535;
                        errno = 0;
                        game_sock = new ServerSocket(next_port, false);
                    } while(errno != 0);

                    // Get rid of temp socket
                    delete game_sock;
                    game_sock = NULL;

                    log(listen_fp, "Next available port found: %d", next_port);

                    // Send new port back to caller:
                    handshakeResponse.command = PORT_HANDSHAKE;
                    handshakeResponse.port_handshake.gamePort = next_port;
                    log(listen_fp, "Sending game port to caller: %d", next_port);
                    listener_sock->send((void*) &handshakeResponse, sizeof(msg_t));

                    log(listen_fp, "Gameplay!");

                    // Spawn new thread for new game

                    Bingo *new_game = new Bingo();
                    thread *player_thread = new thread(play, next_port, new_game);

                    // Reclose log file to write everything
                    fclose(listen_fp);

                    break;
            }
        } else {
            listener_sock->stop();
            listener_sock->start();
        }
    }

    fclose(listen_fp);
}

/**
 * Play a game as a Player; run in separate thread
 */
void play(unsigned int port, Bingo *game)
{
    // Open log file
    char *name = (char*) malloc((BUFMAX+11)*sizeof(char));
    sprintf(name, "player-%s.txt", me->getName().c_str());
    FILE *play_fp = fopen(name, "w+");
    if (!play_fp) {
        error("Could not open player log file for player \"%s\"!", me->getName().c_str());
        return;
    }

    // Create socket with negotiated port
    ServerSocket *game_sock = new ServerSocket(port, false);
    game_sock->start();

    // Play Bingo with caller
    game->playBingo(game_sock, play_fp);

    // Close log file
    fclose(play_fp);
}

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
    /****************
     * HOUSEKEEPING *
     ****************/

     srand(time(NULL)); // help randomize the board values

    // Arg checking.
    if (argc != 3) {
        cprintf(stdout, BOLD, "Usage: ");
        fprintf(stdout, "%s <server_ip> <port>\n", argv[0]);
        exit(FAILURE);
    }

    // Verify IP address.
    int status;
    struct sockaddr_in addr;
    status = inet_pton(AF_INET, argv[1], &addr.sin_addr);
    if (status != 1) {
        error("Invalid IP address!");
        exit(FAILURE);
    }
    strncpy(mgr_ip, argv[1], BUFMAX);   // Store for later

    // Try to convert port.
    errno = 0;
    long int tmp = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        error("Invalid port number.");
        exit(FAILURE);
    }
    if (0 <= tmp && tmp <= 65535) {
        mgr_port = (unsigned short) tmp;
    } else {
        error("Port must be between 0 and 65535.");
        exit(FAILURE);
    }

    Bingo *bng = new Bingo();

    /****************
     * REGISTRATION *
     ****************/
    char *p_name, *p_ip;
    unsigned int p_port;
    getPeerInfo(&p_name, &p_ip, &p_port);

    // Create socket for registration
    bingo_sock = new ClientSocket(mgr_ip, mgr_port, true);

    // Establish connection with manager
    info("Establishing connection with manager...");
    bingo_sock->start();

    // Create player
    me = new Peer(p_name, p_ip, p_port);

    // Attempt to register player with manager
    me->regist(bingo_sock);

    // Close connection with manager
    info("Closing connection with manager...");
    bingo_sock->stop();
    delete bingo_sock;
    bingo_sock = NULL;

    // Create and start listener thread
    thread listener_thread(listener, bng);

    /********
     * MENU *
     ********/
    int choice = -1;
    int kValue = 1;
    char *choice_str;
    msg_t data;
    msg_t handshakeResponse;
    Bingo *caller_bingo;
    Caller *caller_me;
    ClientSocket *tmp_sock;

    // Forever get user's choice
    for ( ; ; ) {
        // Get user's choice
        printMenu();
        choice = getChoice();

        // Decide what to do
        switch (choice) {
            // Exit program
            case 0:
                cprintf(stdout, BOLD, "Exit\n");
                exit(SUCCESS);
                break;  // <-- Here for aesthetic purposes :)

            // Start a game with k players
            case 1:
                // Check if player is registered
                if (!me->isRegistered()) {
                    cprintf(stdout, BOLD, "Player not registered. Please register first.\n");
                    break;
                }

                cprintf(stdout, BOLD, "Start Game\n");

                // Establish connection with manager
                bingo_sock = new ClientSocket(mgr_ip, mgr_port, true);
                info("Establishing connection with manager for Game Setup ...");
                bingo_sock->start();

                // Get user input
                cprintf(stdout, BOLD, "Number of Players: ");
                choice_str = read_line();

                // Convert choice
                errno = 0;
                tmp = strtol(choice_str, NULL, 10);
                while (errno != 0 || tmp == -1) {
                    cprintf(stdout, BOLD, "Needs to be a positive integer!\n");
                    cprintf(stdout, BOLD, "Number of Players: ");
                    choice_str = read_line();
                    errno = 0;
                    tmp = strtol(choice_str, NULL, 10);
                }
                kValue = (int) tmp;

                // Starting game:
                caller_me = new Caller(me->getName(), me->getIP(), me->getPort());
                caller_bingo = new Bingo();
                caller_bingo->startGame(bingo_sock, kValue, caller_me);
                caller_bingo->checkStatus();

                info("Closing connection with manager...");
                bingo_sock->stop();
                delete bingo_sock;
                bingo_sock = NULL;

                // Perform port negotiation with each player
                info("Negotiating gameplay port numbers for %d players...", caller_bingo->gamingPlayers.size());
                for (int i = 0; i < caller_bingo->gamingPlayers.size(); i++) {
                    // Set each player's new port for communication
                    info("Configuring port for player \"%s\"...", caller_bingo->gamingPlayers[i].getName().c_str());
                    caller_bingo->gamingPlayers[i].setPort(caller_bingo->negotiateGameplayPort(caller_bingo->gamingPlayers[i], me->getPort()));

                    // Create socket for player
                    info("Creating socket for player \"%s\"...", caller_bingo->gamingPlayers[i].getName().c_str());
                    info("IP: %s\t Port: %d", caller_bingo->gamingPlayers[i].getIP().c_str(), caller_bingo->gamingPlayers[i].getPort());
                    tmp_sock = new ClientSocket(caller_bingo->gamingPlayers[i].getIP(), caller_bingo->gamingPlayers[i].getPort(), true);
                    caller_bingo->player_socks.push_back(tmp_sock);
                }

                info("GAMEPLAY!");

                // Calling numbers until GAMEOVER:
                caller_bingo->callBingo();

                break;

            // Deregister player
            case 2:
                // Check if player is registered
                if (!me->isRegistered()) {
                    cprintf(stdout, BOLD, "Player not registered.\n");
                    break;
                }

                bingo_sock = new ClientSocket(mgr_ip, mgr_port, true);

                info("Establishing connection with manager...");
                bingo_sock->start();

                status = me->deregist(bingo_sock);

                info("Closing connection to manager...");
                bingo_sock->stop();
                delete bingo_sock;
                bingo_sock = NULL;

                break;

            // Reregister a player
            case 3:
                // Check if player is already registered
                if (me->isRegistered()) {
                    cprintf(stdout, BOLD, "Player already registered.\n");
                    break;
                }

                bingo_sock = new ClientSocket(mgr_ip, mgr_port, true);

                info("Establishing connection with manager...");
                bingo_sock->start();

                // Create player
                me = new Player(p_name, p_ip, p_port);

                // Attempt to register player with manager
                me->regist(bingo_sock);

                // Close connection with manager
                info("Closing connection with manager...");
                bingo_sock->stop();
                delete bingo_sock;
                bingo_sock = NULL;
                break;

            // Query players
            case 4:
                cprintf(stdout, BOLD, "Query Players\n");

                // Connect to manager
                bingo_sock = new ClientSocket(mgr_ip, mgr_port, true);
                info("Establishing connection with manager...");
                bingo_sock->start();

                // Attempt to query players
                bng->queryPlayers(bingo_sock);

                // Close connection with manager
                info("Closing connection with manager...");
                bingo_sock->stop();
                delete bingo_sock;
                bingo_sock = NULL;

                break;

            // Any other choice
            default:
                cprintf(stdout, BOLD, "No such choice on menu.\n");
                break;
        }
    }

    return SUCCESS;
}// end of main()

/*
 * Class Implementations
 */

/***********
 * Bingo *
 ***********/

/**
 * Create a new Bingo
 */
Bingo::Bingo()
{
}

bool Bingo::checkRepeatedValue(int value, vector<int>list, int listSize){
    for (int i = 0; i < listSize; i ++){
        if (value == list[i])
            return true;
    }
    return false;
}

/**
 * Call a number to a player; return true if there is a winner
 */
bool Bingo::call(ClientSocket *sock, int num)
{
    msg_t call_msg, call_rsp;
    ssize_t size;

    // Populate call
    call_msg.command = BINGOCALL;
    call_msg.clr_cmd_bingocals.bingoNumber = num;

    // Send call to player
    info("Calling %d...", num);
    size = sock->send((void*) &call_msg, sizeof(msg_t));

    // Receive ACK from player
    size = sock->receive((void*) &call_rsp, sizeof(msg_t));

    // Check for winner
    if (call_rsp.command == GAMEOVER)
        return true;
    else
        return false;
}

/**
 * Calls Numbers to Players until there is a Winner
 */
void Bingo::callBingo()
{
    ssize_t n;
    int value;
    bool gameOver = false;
    vector<int>calledNumbers;
    int calledNumbersCount = 0;

    msg_t callMessage; 	    // message for sending
	msg_t playerResponse;	// message to receive ACK from player

    // Start sockets
    for (size_t i = 0; i < player_socks.size(); ++i) {
        // Wait until player has set up their socket
        info("Waiting for player \"%s\"...", gamingPlayers[i].getName().c_str());
        do {
            errno = 0;
            player_socks[i]->start();
        } while (errno != 0);
    }

    while (!gameOver){
        // Populating Call Message:
        callMessage.command = BINGOCALL;

        // Generating new random value:
        value = rand() % 75;
        while (checkRepeatedValue(value, calledNumbers, calledNumbersCount)){
            value = rand() % 75;
        }

        // Updating list of called numbers:
        calledNumbers.push_back(value);
        calledNumbersCount++;

        // Call to all players
        for (size_t i = 0; i < player_socks.size(); ++i) {
            if (call(player_socks[i], value)){
                info("GAMEOVER");
                gameOver = true;
                // Send GAMEOVER signal to players PENDING!!!!!!!!!!!!!!!!!!
            }
        }
    }

    // Stop sockets
    for (size_t i = 0; i < player_socks.size(); ++i) {
        player_socks[i]->stop();
    }
}

/**
 * Listens to numbers until GAMEOVER:
 */
void Bingo::playBingo(ServerSocket *sock, FILE *fp)
{
    ssize_t n;
    int inputCode;
    int calledNumber;
    bool gameOver = false;
    // srand(time(NULL));

    Board gameBoard = Board();
    gameBoard.logBoard(fp);

    msg_t inputMessage;
    msg_t callerACK;
    callerACK.command = PLAYERACK;

    while (!gameOver){
        // Listening:
        n = sock->receive((void*) &inputMessage, sizeof(msg_t));
        // info("Receiving %d bytes over socket. Command Code: %d", n, inputMessage.commandCode);
        inputCode = inputMessage.command;

        // Checking command code:
        if (inputCode == BINGOCALL) {
           calledNumber = inputMessage.clr_cmd_bingocals.bingoNumber;
           log(fp, "Number called: %d", calledNumber);

           // Updating GameBoard:
           gameBoard.markNumber(calledNumber);

           // Checking for Win:
           gameOver = gameBoard.checkWin();
           if (gameOver){
               gameBoard.logBoard(fp);
               gameOver = true;
               callerACK.command = GAMEOVER;
               n = sock->send((void*) &callerACK, sizeof(msg_t));
               log(fp, "Sending GAMEOVER signal.");
               // info("Sending GAMEOVER %d bytes over socket.", n);
               return;
           }

           // Sending ACK to caller:
           n = sock->send((void*) &callerACK, sizeof(msg_t));
        }
   }// end of while
}// end of playBingo

/**
 * Sends START_GAME_K command to Manager and stores players:
 */
void Bingo::startGame(ClientSocket *sock, int inputK, Caller *currentPlayer)
{
    // Starting Game:
    // Populating message body:
    msg_t startGameCmd;
    ssize_t n;
    startGameCmd.command = START_GAME;
    startGameCmd.clr_cmd_startgame.k = inputK;
    strncpy(startGameCmd.clr_cmd_startgame.callerIP, currentPlayer->getIP().c_str(), BUFMAX);
    startGameCmd.clr_cmd_startgame.callerPort = currentPlayer->getPort();

    // Sending 'Start game K' command:
    info("Sending START_GAME %d...", inputK);
    n = sock->send((void*) &startGameCmd, sizeof(msg_t));

    // Receiving K Players:
    string newIP, newName;
    int newGameID, newPort;
    msg_t callerACK;
    callerACK.command = CALLERACK;
    msg_t mgrResponse;

    info("Looking for players...");
    for (int i = 0; i < inputK; i++){
        // startGameResponse response;
        n = sock->receive((void*) &mgrResponse, sizeof(msg_t));
        if (mgrResponse.command == FAILURE){
            error("Not enough registered players!");
            return;
        }

        newGameID = mgrResponse.mgr_rsp_startgame.gameID;
        newIP = mgrResponse.mgr_rsp_startgame.playerIP;
        newName = mgrResponse.mgr_rsp_startgame.playerName;
        newPort = mgrResponse.mgr_rsp_startgame.playerPort;

        info("Receiving player %s...", newName.c_str());
        PlayerData tempPlayer(newName, newIP, newPort);
        gamingPlayers.push_back(tempPlayer);

        // Sending ACK back to manager:
        n = sock->send((void*) &callerACK, sizeof(msg_t));
        info("ACK sent to manager.");
    }
}

/**
 * Query all registered players from manager and print them
 */
void Bingo::queryPlayers(ClientSocket *sock)
{
    ssize_t status;

    // Populate command body
    msg_t query;
    query.command = QUERY_PLAYERS;

    // Initialize response
    msg_t response;

    // Send command
    info("Sending QUERY_PLAYERS...");
    status = sock->send((void*) &query, sizeof(msg_t));

    // Look for response
    status = sock->receive((void*) &response, sizeof(msg_t));

    // Decide what to do
    if (status > 0) {
        // Failure
        if (response.command == FAILURE) {
            error("No registered players!");
        } else if (response.command == SUCCESS) {
            info("Receiving players from manager...");
            info("NAME\t\tIP\t\tPort");

            // Print first player sent
            info("%s\t\t%s\t\t%d", response.mgr_rsp_queryplayers.name, response.mgr_rsp_queryplayers.ip, response.mgr_rsp_queryplayers.port);

            // Keep track of how many players left to expect
            int players_left = response.mgr_rsp_queryplayers.players_left;

            // Receive the rest of the players
            while (players_left > 0) {
                // Receive next player
                sock->receive((void*) &response, sizeof(msg_t));

                // Print player
                info("%s\t\t%s\t\t%d", response.mgr_rsp_queryplayers.name, response.mgr_rsp_queryplayers.ip, response.mgr_rsp_queryplayers.port);

                // See how many players are left
                players_left = response.mgr_rsp_queryplayers.players_left;
            }
        } else {
            error("Manager returned unknown data.");
        }
    } else {
        error("No data received from manager!");
    }
}

/*
 * Shows the players that the caller receives after sending "start game K":
 * (Troubleshooting)
 */
void Bingo::checkStatus(){
    info("Number of Gaming Players: %d", this->gamingPlayers.size());
    info("Name:\tIP Address:\tPort:");
    for (int i = 0; i < this->gamingPlayers.size(); i ++){
        info("%s\t%s\t\t%d", gamingPlayers[i].getName().c_str(), gamingPlayers[i].getIP().c_str(), gamingPlayers[i].getPort());
    }
}


/**
 * Caller negotiates gameplay port numbers with input player:
 */
unsigned int Bingo::negotiateGameplayPort(PlayerData player, unsigned int inputCallerGamePort){

    // Initializing socket to talk to default player socket:
    ClientSocket *playerSocket = new ClientSocket(player.getIP(), player.getPort(), true);
    playerSocket->start();

    ssize_t n;
    msg_t callerMessage;
    msg_t playerResponse;
    unsigned int playerGamePort;
    callerMessage.command = PORT_HANDSHAKE;
    callerMessage.port_handshake.gamePort = inputCallerGamePort;   // TEMPORAL!!!!!!

    // Sending callerGamePort to player:
    info("Sending caller's game port %d to player...", inputCallerGamePort);
    n = playerSocket->send((void*) &callerMessage, sizeof(msg_t));
    info("Sent %d bytes from socket.", n);

    // Receiving playerGamePort
    n = playerSocket->receive((void*) &playerResponse, sizeof(msg_t));
    info("Received %d bytes from socket.", n);

    playerGamePort = playerResponse.port_handshake.gamePort;
    info("Received player's new game port: %d", playerGamePort);
    info("Closing connection with manager...");
    playerSocket->stop();
    delete playerSocket;
    playerSocket = NULL;

    return playerGamePort;

}
