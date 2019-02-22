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
#include <vector>
#include "colors.hpp"
#include "bingo.hpp"
#include "cmd.hpp"
#include "constants.hpp"
#include "client.hpp"
#include "server.hpp"
#include "input.hpp"
#include "player.hpp"
#include "peer.hpp"

using namespace std;

/**
 * Log an info message to stdout
 * from the manager.
 */
void info(const char *fmt, ...)
{
    va_list vaList;
    cprintf(stdout, BOLD, "[PLR] ");
    va_start(vaList, fmt);
    fprintf(stdout, fmt, vaList);
    va_end(vaList);
    fprintf(stdout, "\n");
}

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
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
    fprintf(stderr, fmt, vaList);
    va_end(vaList);
    fprintf(stderr, "\n");
}

short ConvertPort(char *inputPort){
    // Try to convert port.
    unsigned short port;
    errno = 0;
    long int tmp = strtol(inputPort, NULL, 10);
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

    return port;
}

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
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{

    if (argc != 3)
		DieWithError( "Parameter Error: usage: bingo caller <manager-IPaddress> <manager-Port> \n or bingo player <player-Port> " );


    // Try to convert port.
    unsigned short port;
    errno = 0;
    long int tmp = strtol(argv[2], NULL, 10);
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

    Bingo *bng = new Bingo();

    /****************
     * REGISTRATION *
     ****************/
    char *p_name, *p_ip;
    unsigned int p_port;
    getPeerInfo(&p_name, &p_ip, &p_port);

    // Create socket for communication with manager
    ClientSocket *bingo_sock = new ClientSocket(argv[1], port);

    // Establish connection with manager
    info("Establishing connection with manager...");
    bingo_sock->start();

    // Create player
    Player *newPlayer = new Player(p_name, p_ip, p_port);

    // Attempt to register player with manager
    newPlayer->registerToManager(bingo_sock);

    // Close connection with manager
    info("Closing connection with manager...");
    bingo_sock->stop();
    // delete bingo_sock;
    // bingo_sock = NULL;

    /********
     * MENU *
     ********/
    int choice = -1;
    int kValue = 3;

    // Forever get user's choice
    for ( ; ; ) {
        // Get user's choice
        printMenu();
        choice = getChoice();

        // Decide what to do
        switch (choice) {
            case 0:
                cprintf(stdout, BOLD, "Exit\n");
                exit(SUCCESS);
                break;  // <-- Here for aesthetic purposes :)
            case 1:
                cprintf(stdout, BOLD, "Start Game\n");
                bingo_sock->start(); // Reopening socket to manager
                bng->StartGame(bingo_sock, kValue);
                break;
            case 2:
                cprintf(stdout, BOLD, "Deregister\n");
                break;
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

/**
 * Calls Numbers to Players until there is a Winner
 */
void Bingo::CallBingo(ClientSocket *sock)
{
    ssize_t n;
    int value;
    bool gameOver = false;

    msg_t callMessage; 	// message for sending
	msg_t playerResponse;		// message to receive ACK from player

    while (!gameOver){
        // Populating Call Message:
        callMessage.command = BINGOCALL;
		value = rand() % 10;
		callMessage.clr_cmd_bingocals.bingoNumber = value;

        // Calling number:
		cout << "Calling " << value << "\n";
		n = sock->send((void*) &callMessage, sizeof(msg_t));
        // cout << " Sending " << n << " bytes over socket.\n";

        // Receiving ACK from player:
		n = sock->receive((void*) &playerResponse, sizeof(msg_t));
        // cout << "Receiving " << n << " bytes over socket. CommandCode " << playerResponse.commandCode << "\n";

        // Confirming player feedback:
		if (playerResponse.command == PLAYERACK)
			cout << "Player ACK received.\n";
		if (playerResponse.command == GAMEOVER){
            info("GAMEOVER");
            gameOver = true;
        }
    }
}

/**
 * Listens to numbers until the game is over or player wins:
 */
void Bingo::PlayBingo(ServerSocket *sock)
{
    ssize_t n;
    int inputCode;
    int calledNumber;
    bool gameOver = false;

    Board gameBoard = Board();
    gameBoard.printBoard();

    msg_t inputMessage;
    msg_t callerACK;
    callerACK.command = PLAYERACK;

    while (!gameOver){
        // Listening:
        n = sock->receive((void*) &inputMessage, sizeof(msg_t));
        // cout << "Receiving " << n << " bytes over socket. CommandCode " << inputMessage.commandCode << "\n";
        inputCode = inputMessage.command;

        // Checking command code:
        if (inputCode == BINGOCALL){
           calledNumber = inputMessage.clr_cmd_bingocals.bingoNumber;
           cout << "Number Called: " << calledNumber << "\n";

           // Updating GameBoard:
           gameBoard.markNumber(calledNumber);

           // Checking for Win:
           gameOver = gameBoard.checkWin();
           if (gameOver){
               gameBoard.printBoard();
               gameOver = true;
               callerACK.command = GAMEOVER;
               n = sock->send((void*) &callerACK, sizeof(msg_t));
               cout << "Sending GAMEOVER signal.\n";
               // cout << "Sending GAMEOVER " << n << " bytes over socket.\n";
               return;
           }

           n = sock->send((void*) &callerACK, sizeof(msg_t));
           cout << "Sending ACK to Caller.\n";
           // cout << "Sending ACK " << n << " bytes over socket.\n";

        }

   }// end of while
}

/**
 * Sends START_GAME_K command to Manager and stores players:
 */
void Bingo::StartGame(ClientSocket *sock, int inputK){
    // Starting Game:
    // Populating message body:
    msg_t startGameCmd;
    ssize_t n;
    startGameCmd.command = START_GAME;
    startGameCmd.clr_cmd_startgame.k = inputK;

    // Sending 'Start game K' command:
    cout << "Sending START_GAME " << inputK << ".\n";
    n = sock->send((void*) &startGameCmd, sizeof(msg_t));

    // Receiving K Players:
    string newIP;
    int newGameID;
    msg_t callerACK;
    callerACK.command = CALLERACK;
    msg_t mgrResponse;

    for (int i = 0; i < inputK; i++){
        // startGameResponse response;
        cout << "receiving player ...\n";
        n = sock->receive((void*) &mgrResponse, sizeof(msg_t));

        cout << "Receiving Player: GameID = " << mgrResponse.mgr_rsp_startgame.gameID << "\tIP = " <<  mgrResponse.mgr_rsp_startgame.playerIP << "\n";

        // Sending ACK back to manager:
        n = sock->send((void*) &callerACK, sizeof(msg_t));
        cout << "ACK sent to manager.\n";
    }
}
