/**
 * @file bingo.hpp
 *
 * This file contains data structures and definitions for
 * the bingo application.
 */

#ifndef _BINGO_HPP_
#define _BINGO_HPP_

#include "client.hpp"

using namespace std;

/*
 * Functions
 */
void getPeerInfo(char **name_ptr, char **ip_ptr, unsigned int *port_ptr);
void printMenu(void);
int getChoice(void);

#endif
