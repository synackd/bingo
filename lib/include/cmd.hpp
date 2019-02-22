/**
 * @file cmd.hpp
 *
 * Defines command numbers for commands.
 */

#ifndef _CMD_HPP_
#define _CMD_HPP_

#include "constants.hpp"
#include <string>
using namespace std;

//****************
//* Return Codes *
//****************
#define FAILURE        -1   /**< The command failed to complete successfully */
#define SUCCESS         0   /**< The command completed successfully */

//*****************
//* Command Codes *
//*****************
/*
 * "Standard" codes as specified by the
 * instruction document
 */
#define REGISTER        1   /**< Register a player with the manager */
#define DEREGISTER      2   /**< Deregister a player from the manager */
#define QUERY_PLAYERS   3   /**< Query registered players from the manager */
#define QUERY_GAMES     4   /**< Query current games from the manager */
#define START_GAME      5   /**< Tell the manager to set up a new game */
#define END             6   /**< Tell the manager to end an ongoing game */
#define CALLERACK       7
#define BINGOCALL       8
#define PLAYERACK       9
#define GAMEOVER        10

/*
 * "Meta" codes defined by project
 * collaborators
 */
// Manager-Player codes

// Player-Caller codes

//***********************
//* Payload Definitions *
//***********************

/*
 * REGISTER
 */

/**
 * @brief The payload to the manager for the
 * 'register' command.
 */
typedef struct {
    char name[BUFMAX];  /**< The name of a player to register */
    char ip[BUFMAX];    /**< The default IP address of the player */
    unsigned int port;  /**< The default port of the player */
} mgr_cmd_register_t;

/**
 * @brief The response from the manager for the
 * 'register' command.
 */
typedef struct {
    int ret_code;   /**< Return code of the previously sent 'register command */
} mgr_rsp_register_t;

/*
 * DEREGISTER
 */

/**
 * @brief The payload to the manager for the
 * 'deregister' command.
 */
typedef struct {
    char name[BUFMAX];  /**< The name of the player to deregister */
} mgr_cmd_deregister_t;

/**
 * @brief The response from the manager for the
 * 'deregister' comand.
 */
typedef struct {
    int ret_code;   /**< Return code of the sent 'deregister' command */
} mgr_rsp_deregister_t;

/*****************************
 * GENERIC PAYLOAD STRUCTURE *
 *****************************/

/**
 * @brief A generic struct for the recipient to read and
 * determine what type of structure to use based on the
 * command code.
 */
typedef struct {
    int command;
    union {
        /* Register command/response */
        mgr_cmd_register_t mgr_cmd_register;    /**< Register player command data */
        mgr_rsp_register_t mgr_rsp_register;    /**< Register player response data */

        /* Deregister command/resoonse */
        mgr_cmd_deregister_t mgr_cmd_deregister;    /**< Deregister player command data */
        mgr_rsp_deregister_t mgr_rsp_deregister;    /**< Deregister player response data */
    };
} msg_t;

#endif
