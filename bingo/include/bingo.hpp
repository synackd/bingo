/**
 * @file bingo.hpp
 *
 * This file contains data structures and definitions for
 * the bingo application.
 */

#ifndef _BINGO_HPP_
#define _BINGO_HPP_

#include "constants.hpp"
#include "client.hpp"

using namespace std;

/*
 * Data
 */
char mgr_ip[BUFMAX];    /**< Holds the manager's IP address */

vector<Player> players; /**< List of in-game players */
Game myGame;            /**< This peer's game */
Player me;              /**< Holds this peer's player data */

/*
 * Functions
 */
void getPeerInfo(char **name_ptr, char **ip_ptr, unsigned int *port_ptr);
void printMenu(void);
int getChoice(void);

#endif
