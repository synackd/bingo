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
#include "cmd.hpp"

using namespace std;

/*
 * Function Prototypes
 */
void info(const char *fmt, ...);
void error(const char *fmt, ...);

/*
 * Class Prototypes
 */

class Manager{
    public:
        int numberOfRegPlayers;
        vector<PlayerData> registeredPlayers;


        // TODO: These methods need implementation
        Manager(void);
        void sendKPlayers(void);
        int registerPlayer(string name, string ip, unsigned int port);
};

#endif
