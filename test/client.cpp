#include        <sys/socket.h>  /* for socket() and bind() */
#include        <stdio.h>               /* printf() and fprintf() */
#include        <stdlib.h>              /* for atoi() and exit() */
#include        <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include        <sys/types.h>
#include        <string.h>
#include        <unistd.h>
#include <iostream>

using namespace std;

#define ECHOMAX 255             /* Longest string to echo */
#define BACKLOG 128
#define STARTGAME 10

struct message{
    int commandCode;
    int parameters;
};

void DisplayMenu()
{
    printf("Welcome to Bingo!\n");
    printf("Type \"start game k\" to request a game with k players.\n");
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
        bool messageReady = false;

        // Converting char[] into String for processing:
        string inputCommand(sendline);

        // Initializing foudnIndex so it's not found by default.
        int foundIndex = -1;

        // Checking length of command to check for proper command:
        int commandLength = inputCommand.length();

        if (commandLength > 9){
            // Checking for "start game" command
            foundIndex = inputCommand.find("start game");

            if (foundIndex >= 0){
                // Parsing command:
                // 'start game' has 10 characters
                std::size_t kIndex = foundIndex + 10;
                string kStr = inputCommand.substr(kIndex);
                int kInt = stoi(kStr);
                printf("sending \"Start Game - %i\"\n", kInt);

                // Populating message body:
                outputMessage.commandCode = STARTGAME;
                outputMessage.parameters = kInt;
                messageReady = true;
            }
        }else{
            printf("Invalid command. Trye again.\n");
        }

        // Sending message if needed:
        if (messageReady){
            write(sockfd, &outputMessage, sizeof(message));
            if ((n = read(sockfd, recvline, ECHOMAX)) == 0)
              DieWithError("str_cli: server terminated prematurely");
            // recvline[ n ] = '\0';
            fputs(recvline, stdout);
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
