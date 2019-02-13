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
#define MAXPLAYERS 20   // TEMPORAL!!!


using namespace std;

struct player{
	string IP;
	int Port;
};

struct startGameResponse{
	int GameID;
	int playersCount;
	player GamePlayers[MAXPLAYERS];
};



int main(int argc, char **argv)
{
	string sendline = "start game ";
	int found = -1;

	int command_length = sendline.length();
	printf("length: %i\n",command_length );

	if(command_length >= 10){

		found = sendline.find("start game");
		printf("found: %i\n", found);
	}

	//
	if (found >= 0){
		printf("Start game command detected. \n");

		// 'start game' has 10 characters


		try{
			int k_index = found + 10;
			string k_str = sendline.substr(k_index);
			int k_int = stoi(k_str);
			printf("K value: %i\n", k_int);
		}catch(...){
			printf("Check format.\n");
		}




	}else{
		printf("Invalid command. \n");

	}

	// string parsed = sendline.substr()

}
