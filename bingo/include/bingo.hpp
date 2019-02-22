/**
 * @file bingo.hpp
 *
 * This file contains data structures and definitions for
 * the bingo application.
 */

#ifndef _BINGO_HPP_
#define _BINGO_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "colors.hpp"
#include "player.hpp"
#include "client.hpp"
#include "server.hpp"
#include "cmd.hpp"

using namespace std;

/*
 * Functions
 */
void info(const char *fmt, ...);
void error(const char *fmt, ...);

/*
 * Class Prototypes
 */

class Bingo{
    public:
        int numberOfGamingPlayers;
        vector<PlayerData> gamingPlayers;


        // TODO: These methods need implementation
        Bingo(void);
        void CallBingo(ClientSocket *sock);
        void PlayBingo(ServerSocket *sock);
        void StartGame(ClientSocket *sock, int inputK);
        void CheckStatus();
};

#endif
