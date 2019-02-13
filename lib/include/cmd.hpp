/**
 * @file cmd.hpp
 *
 * Defines command numbers for commands.
 */

#ifndef _CMD_HPP_
#define _CMD_HPP_

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

/*
 * "Meta" codes defined by project
 * collaborators
 */
// Manager-Player codes

// Player-Caller codes

//***********************
//* Payload Definitions *
//***********************
/**
 * @brief A generic struct for the recipient to read and
 * determine what type of structure to use based on the
 * command code.
 */
typedef struct {
    int command;
} any_cmd_t;

/**
 * @brief The payload to the manager for the
 * 'register' command.
 */
typedef struct {
    int command;        /**< The command code for the manager */
    char[BUFMAX] name;  /**< The name of a player to register/deregister */
    char[BUFMAX] ip;    /**< The default IP address of the player */
    char[BUFMAX] port;  /**< The default port of the player */
} mgr_cmd_register_t;

/**
 * @brief The response from the manager for the
 * 'register' command.
 */
typedef struct {
    int ret_code;       /**< Return code by the previously sent command */
    int game_uid;       /**< The queried unique game identifier */
} mgr_rsp_register_t;


#endif
