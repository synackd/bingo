#include        <sys/socket.h>  /* for socket() and bind() */
#include        <stdio.h>               /* printf() and fprintf() */
#include        <stdlib.h>              /* for atoi() and exit() */
#include        <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include        <sys/types.h>
#include        <string.h>
#include        <unistd.h>
#include <iostream>
#include <list>
#include <iterator>
#include "bingo.h" // Contains structs and classes

using namespace std;

//Global Variables:


void DisplayMenu()
{
    printf("Welcome to Bingo!\n");
    printf("Type \"start game k\" to request a game with k players.\n");
    printf("Type ");
}

void DieWithError(const char *errorMessage) /* External error handling function */
{
  perror(errorMessage);
  exit(1);
}

void str_cli(FILE *fp, int sockfd)
{
  	ssize_t n;
    char    sendline[ECHOMAX], recvline[ECHOMAX];

    while (fgets(sendline, ECHOMAX, fp) != NULL) {

        message outputMessage;

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
                    printf("sending \"Start Game - %i\"\n", kInt);

                    // Populating message body:
                    outputMessage.commandCode = STARTGAME;
                    outputMessage.parameters = kInt;

                    // Sending 'Start game K' command:
                    n = write(sockfd, &outputMessage, sizeof(message));
                    if (n < 0)
                        DieWithError("ERROR writing to socket");

                    // Receiving K Players:
                    startGameResponse response;

                    for (int i = 0; i < kInt; i++){
                        // startGameResponse response;
                        n = read(sockfd, &response, sizeof(startGameResponse));
                        if (n < 0)
                            DieWithError("ERROR reading from socket");
                        else{

                            printf("Received Player: GameID=%d\t IP=%c\t Port=%d\n", response.gameID, response.playerIP, response.playerPort);
                            printf("ACK sent to manager.\n");

                            // Sending ACK back to manager:
                            outputMessage.commandCode = CALLERACK;
                            n = write(sockfd, &outputMessage, sizeof(message));
                            if (n < 0)
                                DieWithError("ERROR writing to socket");


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

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 3)
		DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    DisplayMenu();

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
