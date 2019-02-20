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

#define STARTGAME 10
#define CALLERACK 1
#define BINGOCALL 2
#define PLAYERACK 3
#define GAMEOVER 4

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
        // TODO: These methods need implementation
        Manager(void);
        void sendKPlayers(void);
        void registerPlayer(void);
};

#endif
