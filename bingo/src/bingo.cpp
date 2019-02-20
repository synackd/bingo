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

void caller()
{
    Board testBoard = Board();

    testBoard.printBoard();

    int value;
    bool gameover = false;

    while (!gameover){
        value = rand() % 10;
        info("Calling %d", value);
        testBoard.markNumber(value);
        gameover = testBoard.checkWin();
    }

    info("GAMEOVER");
    testBoard.printBoard();
}

void player(int port)
{
	ssize_t n;
    int inputCode;
    int calledNumber;
	bool gameOver = false;

    Board testBoard = Board();
    message *inputMessage = (message*) malloc(sizeof(message));
	message *callerACK = (message*) malloc(sizeof(message));
	callerACK->commandCode = PLAYERACK;
    ServerSocket sock = new ServerSocket(port);
    sock.start();

    for ( ; ; ) {
	    if ( (n = sock->receive((void**) &inputMessage, sizeof(message))) == 0 )
   	    	return; /* connection closed by other end */

        inputCode = inputMessage->commandCode;

        if (inputCode == BINGOCALL){
            info("Bingo Call received!");
			calledNumber = inputMessage->parameters;
            info("Number: %d", calledNumber);

			gameBoard.CheckNumber(calledNumber);
	        gameOver = gameBoard.checkWin();

			// updating command code if player wins:
			if (gameOver){
				callerACK->commandCode = GAMEOVER;

				gameBoard.printBoard();
			}

			n = sock->send((void**) &callerACK, sizeof(message));
			if (n < 0) {
                // TODO: Implement return code here
				error("Could not write to socket!");
                return;
            }
			info("ACK/GAMEOVER sent to caller.");
        }
    }// end of for loop
}

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
}
