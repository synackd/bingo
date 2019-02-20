#ifndef _COLORS_HPP_
#define _COLORS_HPP_

#include <cstdio>

#define RESET           "\x1b[0m"
#define BOLD            "\x1b[1m"
#define ITALIC          "\x1b[3m"
#define UNDERLINE       "\x1b[4m"
#define REVERSED        "\x1b[7m"
#define STRIKETHROUGH   "\x1b[9m"

/*
 * FOREGROUND
 */
// 8 Colors
#define FG_BLACK    "\x1b[30m"
#define FG_RED      "\x1b[31m"
#define FG_GREEN    "\x1b[32m"
#define FG_YELLOW   "\x1b[33m"
#define FG_BLUE     "\x1b[34m"
#define FG_MAGENTA  "\x1b[35m"
#define FG_CYAN     "\x1b[36m"
#define FG_WHITE    "\x1b[37m"

// 16 Colors
#define FG_BRIGHT_BLACK     "\x1b[30;1m"
#define FG_BRIGHT_RED       "\x1b[31;1m"
#define FG_BRIGHT_GREEN     "\x1b[32;1m"
#define FG_BRIGHT_YELLOW    "\x1b[33;1m"
#define FG_BRIGHT_BLUE      "\x1b[34;1m"
#define FG_BRIGHT_MAGENTA   "\x1b[35;1m"
#define FG_BRIGHT_CYAN      "\x1b[36;1m"
#define FG_BRIGHT_WHITE     "\x1b[37;1m"

/*
 * BACKGROUND
 */
// 8 Colors
#define BG_BLACK    "\x1b[40m"
#define BG_RED      "\x1b[41m"
#define BG_GREEN    "\x1b[42m"
#define BG_YELLOW   "\x1b[43m"
#define BG_BLUE     "\x1b[44m"
#define BG_MAGENTA  "\x1b[45m"
#define BG_CYAN     "\x1b[46m"
#define BG_WHITE    "\x1b[47m"

// 16 Colors
#define BG_BRIGHT_BLACK     "\x1b[40;1m"
#define BG_BRIGHT_RED       "\x1b[41;1m"
#define BG_BRIGHT_GREEN     "\x1b[42;1m"
#define BG_BRIGHT_YELLOW    "\x1b[43;1m"
#define BG_BRIGHT_BLUE      "\x1b[44;1m"
#define BG_BRIGHT_MAGENTA   "\x1b[45;1m"
#define BG_BRIGHT_CYAN      "\x1b[46;1m"
#define BG_BRIGHT_WHITE     "\x1b[47;1m"

void cprintf(FILE * stream, const char *color, const char * fmt, ...);

#endif
