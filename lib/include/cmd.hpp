/**
 * @file cmd.hpp
 *
 * Defines command numbers for commands.
 */

#ifndef _CMD_HPP_
#define _CMD_HPP_

#include <string>
#include "constants.hpp"

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
    int ret_code;   /**< Return code of the previously sent 'register' command */
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

/*
 * START GAME
 */

/**
 * Bingo Call command
 */
typedef struct clr_cmd_bingocall_t {
    int bingoNumber;    /**< The number being called (e.g. 'B7') */
} clr_cmd_bingocall_t;

/**
 * Player response to Bingo Call
 */
typedef struct ply_rsp_bingocall_t {
    int ret_code;       /**< Return code from caller for bingo call */
} ply_rsp_bingocall_t;

/**
 * Start Game command
 */
typedef struct clr_cmd_startgame_t {
    int k;              /**< Number of players requested for new bingo game */
} clr_cmd_startgame_t;

/**
 * Manager's response to Start Game command
 */
typedef struct {
    int gameID;                 /**< New game's ID (if started) */
    int playersLeft;            /**< Sentinel for caller to know how many responses left */
    char playerIP[BUFMAX];      /**< Player's default IP address */
    char playerName[BUFMAX];    /**< Player's name */
    unsigned int playerPort;    /**< Player's default port */
} mgr_rsp_startgame_t;

/*
 * QUERY PLAYERS
 */

/**
 * Query Players response from manager
 */
typedef struct {
    int players_left;       /**< Sentinel for peer to know how many responses are left */
    char name[BUFMAX];      /**< Player's name */
    char ip[BUFMAX];        /**< Player's default IP address */
    unsigned int port;      /**< Player's default port */
} mgr_rsp_queryplayers_t;

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

        /* Deregister command/response */
        mgr_cmd_deregister_t mgr_cmd_deregister;    /**< Deregister player command data */
        mgr_rsp_deregister_t mgr_rsp_deregister;    /**< Deregister player response data */

        /* Calling command/response (caller-player) */
        clr_cmd_bingocall_t clr_cmd_bingocals;  /**< Bingo Call command data */
        ply_rsp_bingocall_t ply_rsp_bingocall;  /**< Player response to Bingo Call data */

        /* Start game command/response */
        clr_cmd_startgame_t clr_cmd_startgame;  /**< Caller Start Command data */
        mgr_rsp_startgame_t mgr_rsp_startgame;  /**< Manager Response to StartGame command */

        /* Query players response */
        mgr_rsp_queryplayers_t mgr_rsp_queryplayers;    /**< Manager response to Query Players command */
    };
} msg_t;

#endif
