#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <iostream>

#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define STARTGAME 10
#define CALLERACK 1

using namespace std;

class Player{
public:
    string IP;
    int Port;

    Player(string inputIP, int inputPort){
        IP = inputIP;
        Port = inputPort;
    }

    void PrintPlayer(){
        cout << "Player IP: " << IP << "\t Port: " << Port << "\n";
    }

};

struct message{
    int commandCode;
    int parameters;
};

struct startGameResponse{
    int gameID;
    int playersLeft;
    Player gamePlayer;
};
