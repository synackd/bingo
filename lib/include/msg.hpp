/*
 * @file msg.hpp
 *
 * This file contains definitions relating to message
 * exchange between manager and bingo applications.
 */

#ifndef _MSG_HPP_
#define _MSG_HPP_

#define MAXBUF 256  /**< Maximum buffer size */

/**
 * Defines the global message format for both
 * manager and bingo applications.
 */
typedef struct {
    int cmd;                /**< Command being sent */
    int flag;               /**< One of CALL, ACK, or WIN */
    char[MAXBUF] param1;    /**< First parameter of cmd, if any */
    char[MAXBUF] param2;    /**< Second parameter of cmd, if any */
    char[MAXBUF] param3;    /**< Third parameter of cmd, if any */
} msg;

#endif
