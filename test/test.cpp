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
	// LIST OF REGISTERED PLAYERS:
    int numberOfRegPlayers = 5;
    vector<Player> playersList;

    for (int i = 0; i < numberOfRegPlayers; i++){
        string IP = "IP" + std::to_string(i);
        Player tempPlayer(IP, i);
		playersList.push_back(tempPlayer);
    }

	for (int i = 0; i < numberOfRegPlayers; i++){
        playersList.at(i).PrintPlayer();
    }

	startGameResponse testResponse;
	cout << sizeof(startGameResponse);

}
