/**
 * @file bingo.hpp
 *
 * This file contains data structures and definitions for
 * the bingo application.
 */

#ifndef _BINGO_HPP_
#define _BINGO_HPP_

#define STARTGAME 10
#define CALLERACK 1
#define BINGOCALL 2
#define PLAYERACK 3
#define GAMEOVER 4

/*
 * Functions
 */
void info(const char *fmt, ...);
void error(const char *fmt, ...);

#endif
