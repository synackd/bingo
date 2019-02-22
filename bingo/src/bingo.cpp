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
#include "client.hpp"
#include "server.hpp"
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

    ClientSocket *cSock;
	cSock = new ClientSocket(argv[1], port);
	cSock->start();

    bng->StartGame(cSock, 3);

	exit(0);
}

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
void CallBingo(ClientSocket *sock)
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
void PlayBingo(ServerSocket *sock)
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
void StartGame(ClientSocket *sock, int inputK){
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
