#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <iostream>
#include <vector>
#include "bingo.h" // Contains structs and classes
using namespace std;

// Global Variables:
vector<Player> playersList;   // List of registered players


void DieWithError(const char *errorMessage) /* External error handling function */
{
	perror(errorMessage);
	exit(1);
}

void EchoString(int sockfd)
{
    ssize_t n;
    char    line[ECHOMAX];
    int inputCode;
    int receivedK;

    message inputMessage;

    for ( ; ; ) {
	    if ( (n = read(sockfd, &inputMessage, sizeof(message))) == 0 )
   	    	return; /* connection closed by other end */

        inputCode = inputMessage.commandCode;

        if (inputCode == STARTGAME){
          printf("Start Game command received!\n");
          printf("Sending %i players to caller.\n", inputMessage.parameters);

          // Sending k players from list --> RANDOM PENDING!!!!!
          int gameID = 1; // RANDOM PENDING!!
          int playersLeft = receivedK;

          for (int i = 0; i < inputMessage.parameters; i++){
              startGameResponse response;
              Player tempPlayer(playersList.at(i).IP, playersList.at(i).Port);
              response.gameID = gameID;
              response.gamePlayer = &tempPlayer;
              response.playersLeft = playersLeft;
              playersLeft --;

			  printf("Sending: \n");
              response.gamePlayer->PrintPlayer();
              write(sockfd, &response, sizeof(startGameResponse));
			  if (n < 0)
			  	DieWithError("ERROR writing to socket");

              // waiting for ACK:
			  n = read(sockfd, &inputMessage, sizeof(message));
			  if (n < 0)
				  DieWithError("ERROR reading from socket");
			  else{
				  if (inputMessage.commandCode == CALLERACK)
				  	printf("Caller ACK received.\n");
			  }

          }
	  } // end of STARTGAME command processing

    }// end of for loop

}// end of EchoString


int main(int argc, char **argv)
{
    int sock, connfd;                /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    // TEMPORAL LIST OF PLAYERS FOR TESTING: //////////////////////

    // LIST OF REGISTERED PLAYERS:
    int numberOfRegPlayers = 5;

    for (int i = 0; i < numberOfRegPlayers; i++){
        string IP = "IP" + std::to_string(i);
        Player tempPlayer(IP, i);
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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("server: socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("server: bind() failed");

	if (listen(sock, BACKLOG) < 0 )
		DieWithError("server: listen() failed");

	printf("Manager listening at port %s\n", argv[1]);

	cliAddrLen = sizeof(echoClntAddr);
	connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );

	EchoString(connfd);
	close(connfd);
}
