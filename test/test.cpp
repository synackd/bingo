#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <iostream>

#include <vector>
#include "bingo.h"
using namespace std;


int main(int argc, char **argv)
{
	// TESTING GAME LIST in caller:
    int MAXGAMES = 10;
    Game List[MAXGAMES];

    int numberOfRegPlayers = 3;

    for (int i = 0; i < numberOfRegPlayers; i++){
        string IP = "IP" + std::to_string(i);
        Player tempPlayer(IP, i);
		playersList.push_back(tempPlayer);
		playerCount ++;
    }



    // Saving Game with 3 players
    for (int i=0; i< 3; i++){

    }



}
