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

void callerRole(ClientSocket *sock)
{
    ssize_t n;
    int value;
    bool gameOver = false;

    message callMessage; 	// message for sending
	message playerResponse;		// message to receive ACK from player

    while (!gameOver){
        callMessage.commandCode = BINGOCALL;
		value = rand() % 10;
		callMessage.parameters = value;

		// Sending 'Start game K' command:
		cout << "calling " << value << "\n";
		n = sock->send((void**) &callMessage, sizeof(message));
		if (n < 0)
			DieWithError("ERROR writing to socket");

		n = sock->receive((void**) &playerResponse, sizeof(message));
		if (n < 0)
			DieWithError("ERROR reading from socket");
		else{

			if (playerResponse.commandCode == PLAYERACK)
				cout << "Player ACK received.\n";

			if (playerResponse.commandCode == GAMEOVER)
				gameOver = true;
		}
    }

    info("GAMEOVER");
}

void playerRole(ServerSocket *sock)
{
    ssize_t n;
    int inputCode;
    int calledNumber;
	bool gameOver = false;

    Board gameBoard = Board();
    gameBoard.printBoard();

    // message inputMessage;
    message *inputMessage = (message*) malloc(sizeof(message)); // Receiving
    message *callerACK = (message*) malloc(sizeof(message)); // Receiving
<<<<<<< HEAD
	// message callerACK;
=======
	message callerACK;
>>>>>>> a78e49e8266459a77920b0919011a1706521bbf0
	callerACK->commandCode = PLAYERACK;

    for ( ; ; ) {
	    if ( (n = sock->receive((void**) &inputMessage, sizeof(message))) == 0 )
   	    	return; /* connection closed by other end */

        inputCode = inputMessage->commandCode;

        if (inputCode == BINGOCALL){
            printf("Bingo Call received!\n");
			calledNumber = inputMessage->parameters;
            cout << "Number: " << calledNumber << "\n";

			gameBoard.markNumber(calledNumber);
	        gameOver = gameBoard.checkWin();

			// updating command code if player wins:
			if (gameOver){
				callerACK->commandCode = GAMEOVER;
				gameBoard.printBoard();
			}

			n = sock->send((void**) &callerACK, sizeof(message));
			if (n < 0)
				DieWithError("ERROR writing to socket");
			cout << "ACK/GAMEOVER sent to caller.\n";

        }

    }// end of for loop
}

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
    cout << "ArgCount = " << argc <<"\n";

    if (argc != 4 && argc != 3)
		DieWithError( "Parameter Error: usage: bingo caller <player-IPaddress> <player-Port> \n or bingo player <player-Port> " );

    // Checking role type;
    if (strcmp(argv[1],"caller") == 0) {
        cout << "Caller Role Started:\n";
        ClientSocket *cSock;
    	cSock = new ClientSocket(argv[2], atoi(argv[3]));
    	cSock->start();
        callerRole(cSock);
    }else if(strcmp(argv[1],"player") == 0){
        cout << "Player Role Started:\n";
        ServerSocket *pSock;
    	pSock = new ServerSocket(atoi(argv[2]));
    	pSock->start();
        playerRole(pSock);
    }else{
        cout << "Invalid Role value. Valid parameters are ./client -IPaddress -Port -Role\n";
        exit(0);
    }

	exit(0);
}
