/**
 * @file bingo.hpp
 *
 * This file contains data structures and definitions for
 * the bingo application.
 */

#ifndef _BINGO_HPP_
#define _BINGO_HPP_

#include "constants.hpp"
#include "server.hpp"
#include "client.hpp"
#include "player.hpp"
#include "peer.hpp"

using namespace std;

/*
 * Functions
 */
void getPeerInfo(char **name_ptr, char **ip_ptr, unsigned int *port_ptr);
void printMenu(void);
int getChoice(void);

/*
 * Class Prototypes
 */

class Bingo{
    public:
        int numberOfGamingPlayers;
        vector<PlayerData> gamingPlayers;

        // TODO: These methods need implementation
        Bingo(void);
        void callBingo(ClientSocket *sock);
        void playBingo(ServerSocket *sock);
        void startGame(ClientSocket *sock, int inputK);
        void queryPlayers(ClientSocket *sock);
        unsigned int NegotiateGameplayPort(PlayerData player, unsigned int inputCallerGamePort);
        void checkStatus();
};

#endif
