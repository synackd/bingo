/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <stdio.h>
#include <stdarg.h>
#include "colors.hpp"
#include "manager.hpp"
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
    cprintf(stderr, BOLD, "[MGR][ERR] ");
    va_start(vaList, fmt);
    fprintf(stderr, fmt, vaList);
    va_end(vaList);
    fprintf(stderr, "\n");
}

void EchoString(ServerSocket *sock)
{
    ssize_t n;
    char    line[ECHOMAX];
    int inputCode;
    int receivedK;

    message *inputMessage = (message*) malloc(sizeof(message));

    for ( ; ; ) {
	    if ( (n = sock->receive((void**) &inputMessage, sizeof(message))) == 0 )
   	    	return; /* connection closed by other end */

        inputCode = inputMessage->commandCode;

        if (inputCode == STARTGAME){
          info("Start Game command received!");
          info("Sending %i players to caller.", inputMessage->parameters);

          // Sending k players from list --> RANDOM PENDING!!!!!
          int gameID = 1; // RANDOM PENDING!!
		  int numberOfPlayersToSend = inputMessage->parameters;
          int playersLeft = receivedK;
		  startGameResponse *response = (startGameResponse*) malloc(sizeof(startGameResponse));

          for (int i = 0; i < numberOfPlayersToSend; i++){

              // Player tempPlayer(playersList.at(i).IP, playersList.at(i).Port);
              response->gameID = gameID;
			  response->playersLeft = playersLeft;
              response->playerIP = playersList[i].playerIP;
              response->playerPort = playersList[i].Port;
              playersLeft --;

			  cout << "Sending Player: GameID = " << response->gameID << "\tIP = " << response->playerIP << "\tPort = " << response->playerPort << "\n";
              // response.gamePlayer->PrintPlayer();
              sock->send((void**) &response, sizeof(startgameResponse));
			  if (n < 0)
			  	error("Could not write to socket!");

              // waiting for ACK:
			  n = sock->receive((void**) &inputMessage, sizeof(message));
			  if (n < 0)
				  error("Could not read from socket!");
			  else{
				  if (inputMessage->commandCode == CALLERACK)
				  	info("Caller ACK received.");
			  }
          }
	  } // end of STARTGAME command processing

    }// end of for loop

}// end of EchoString

/**
 * Main runtime of manager application.
 */
int main(int argc, char **argv)
{
    // TEMPORAL LIST OF PLAYERS FOR TESTING: //////////////////////

    // LIST OF REGISTERED PLAYERS:
    int numberOfRegPlayers = 5;

    for (int i = 0; i < numberOfRegPlayers; i++){
        string playerIP = "IP" + std::to_string(i);
        PlayerData tempPlayer(playerIP, i);
		playersList.push_back(tempPlayer);
    }

    //////////////////////////////////////////////////////////////


    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage: %s <TDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    ClientSocket *sock = new ServerSocket(echoServPort);
    sock->start();
	EchoString(sock);
	sock->stop();
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
void Manager::registerPlayer()
{
}
