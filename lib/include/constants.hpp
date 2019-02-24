/**
 * @file constants.hpp
 *
 * Global constants for the project.
 */

#ifndef _CONSTANTS_HPP_
#define _CONSTANTS_HPP_

#define BUFMAX          100  /**< Maximum buffer size for strings */
#define INPUT_BUFSIZE   1024 /**< Maximum buffer size for user input */

//****************
//* Return Codes *
//****************
#define CANNOTWRITE    -6   /**< Could not write to socket */
#define CANNOTREAD     -5   /**< Reading from socket failed */
#define CANNOTCONN     -4   /**< Socket could not connect */
#define CANNOTLISTEN   -3   /**< Could not listen on socket */
#define CANNOTBIND     -2   /**< Socket could not bind at specified port */
#define FAILURE        -1   /**< The command failed to complete successfully */
#define SUCCESS         0   /**< The command completed successfully */

#endif
