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

void str_cli(FILE *fp, int sock)
{
  	ssize_t n;
    char    sendline[ECHOMAX], recvline[ECHOMAX];

    while (fgets(sendline, ECHOMAX, fp) != NULL) {

        // Converting char[] into String for processing:
        string inputCommand(sendline);

        // Initializing foudnIndex so it's not found by default.
        int foundIndex = -1;

        // Checking length of command to check for proper command:
        int commandLength = inputCommand.length();

        if (commandLength > 10){
            // Checking for "start game" command
            foundIndex = inputCommand.find("start game");

            if (foundIndex >= 0){
                try{
                    // Parsing command:
                    // 'start game' has 10 characters
                    std::size_t kIndex = foundIndex + 10;
                    string kStr = inputCommand.substr(kIndex);
                    int kInt = stoi(kStr);
                    info("Sending \"Start Game - %i\"\n", kInt);

                    // Populating message body:
                    message outputMessage;
                    info("Populating message body...");
                    outputMessage.commandCode = STARTGAME;
                    outputMessage.parameters = kInt;

                    // Sending 'Start game K' command:
                    info("Sending command...");
                    n = sock->send((void**) &outputMessage, sizeof(message));
                    if (n < 0)
                        error("Could not write to socket!");

                    // Receiving K Players:
                    string newIP;
                    int newGameID;
                    message callerACK;
                    callerACK.commandCode = CALLERACK;
                    startGameResponse response;

                    for (int i = 0; i < kInt; i++){
                        // startGameResponse response;
                        info("Receiving player...");
                        n = sock->receive((void**) &response, sizeof(startGameResponse));
                        if (n < 0) {
                            error("Could not read from socket!");
                            return;
                        } else {
                            //newGameID = response.gameID;
                            // newIP = response.playerIP;
                            //
                            // cout << "Receiving Player: GameID = " << newGameID << "\tIP = " << newIP << "\n";
                            // Creating Player
                            Player tempPlayer = new Player(response.playerIP, response.playerPort);
                            cout << tempPlayer.playerIP << "\n";
                            // tempPlayerrintPlayer();
                            // usleep(2000);

                            // Sending ACK back to manager:
                            n = write(sockfd, &callerACK, sizeof(message));
                            if (n < 0)
                                DieWithError("ERROR writing to socket");
                            cout << "ACK sent to manager.\n";

                        }
                    }

                    // Starting new Game:
                    printf("Ready to start game.\n");

                }catch(...){
                    printf("Wrong command format. Check User Guide and try again.\n");
                }
            }
        }else{
            printf("Invalid command. Trye again.\n");
        }

    }// end of while

}// end of str_cli

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
	ClientSocket *sock;

	if (argc != 3)
		DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );

	sock = new ClientSocket(argv[1], atoi(argv[2]));

    DisplayMenu();

	sock->start();

	str_cli(stdin, sock);		/* do it all */

	exit(0);
}
