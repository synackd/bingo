/**
 * @file cmd.hpp
 *
 * Defines command numbers for commands.
 */

#ifndef _CMD_HPP_
#define _CMD_HPP_

#define REGISTER        1   /**< Register a player with the manager */
#define DEREGISTER      2   /**< Deregister a player from the manager */
#define QUERY_PLAYERS   3   /**< Query registered players from the manager */
#define QUERY_GAMES     4   /**< Query current games from the manager */
#define START_GAME      5   /**< Tell the manager to set up a new game */
#define END             6   /**< Tell the manager to end an ongoing game */

#endif
