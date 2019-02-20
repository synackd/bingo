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

vector<Player> playersList;

void DieWithError(const char *errorMessage) /* External error handling function */
{
	perror(errorMessage);
	exit(1);
}

void CallBingo(int sockfd)
{
	ssize_t n;
    int inputCode;
	bool gameOver = false;
	int value;

    message callMessage; 	// message for sending
	message playerResponse;		// message to receive ACK from player

	while (!gameOver){
		// Populating callMessage:
		callMessage.commandCode = BINGOCALL;
		value = rand() % 10;
		callMessage.parameters = value;

		// Sending 'Start game K' command:
		cout << "calling " << value << "\n";
		n = write(sockfd, &callMessage, sizeof(message));
		if (n < 0)
			DieWithError("ERROR writing to socket");

		n = read(sockfd, &playerResponse, sizeof(startGameResponse));
		if (n < 0)
			DieWithError("ERROR reading from socket");
		else{

			if (playerResponse.commandCode == PLAYERACK)
				cout << "Player ACK received.\n";

		}

		usleep(3000);
	} // end of while loop

}// end of EchoString


int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 3)
		DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );

	sockfd = socket(AF_INET, SOCK_STREAM, 0);


	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	CallBingo(sockfd);		/* do it all */

	exit(0);
}
