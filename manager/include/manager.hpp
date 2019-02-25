/**
 * @file manager.hpp
 *
 * This file contains data structures and definitions for
 * the manager application.
 */

#ifndef _MANAGER_H_
#define _MANAGER_H_

#include <iostream>
#include <string>
#include <vector>
#include "colors.hpp"
#include "player.hpp"
#include "server.hpp"
#include "cmd.hpp"
#include "peer.hpp"

using namespace std;

/*
 * Class Prototypes
 */

class Manager {
    public:
        int numberOfRegPlayers;
        vector<PlayerData> registeredPlayers;
        int numberOfGames;
        vector<Game> gameList;

        Manager(void);
        bool checkUsedIDs(int newID, vector<Game> list, int size);
        int generateGameID();
        void sendKPlayers(ServerSocket *sock, msg_t data, Game *gameDetails);
        int sendAllPlayers(ServerSocket *sock);
        int registerPlayer(string name, string ip, unsigned int port);
        int deregisterPlayer(string name);
};

#endif
