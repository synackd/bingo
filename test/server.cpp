#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <iostream>

using namespace std;

#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define STARTGAME 10

struct message{
    char command[ECHOMAX];
    int parameters;
};

void DieWithError(const char *errorMessage) /* External error handling function */
{
	perror(errorMessage);
	exit(1);
}

void EchoString(int sockfd)
{
    ssize_t n;
    char    line[ECHOMAX];
    std::string input;

    message inputMessage;

    for ( ; ; ) {
	    if ( (n = read(sockfd, &inputMessage, sizeof(message))) == 0 )
   	    	return; /* connection closed by other end */

        input = inputMessage.command;

        if (input.compare("start game\n") == 0){
          printf("Start Game command received!\n");

        }else{
            printf("Invalid command.\n");
        }

        write(sockfd, line, n );
    }

}


int main(int argc, char **argv)
{
		int sock, connfd;                /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

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
