/**
 * @file manager.cpp
 *
 * This file contains the main runtime source for the bingo
 * application.
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <vector>
#include "colors.hpp"
#include "log.hpp"
#include "bingo.hpp"
#include "cmd.hpp"
#include "client.hpp"
#include "server.hpp"
#include "peer.hpp"
#include "input.hpp"

using namespace std;

/*
 * Data
 */
ClientSocket *bingo_sock;   /**< Socket for communicating with the manager */

/**
 * Menu system for allowing the peer to decide
 * what to do (e.g. be a caller, deregister, etc.
 */
void printMenu()
{
    // Header
    cprintf(stdout, BOLD, "\n Choices:\n");

    // Register
    cprintf(stdout, BOLD, "  1) ");
    fprintf(stdout, "Register\n");

    fprintf(stdout, "\n");
}

/**
 * Get user's choice after printing menu options
 */
int getChoice()
{
    char *choice_str;
    long int tmp = 0;
    int choice;

    // Get user input
    cprintf(stdout, BOLD, "Your choice: ");
    choice_str = read_line();

    // Convert choice
    errno = 0;
    tmp = strtol(choice_str, NULL, 10);
    while (errno != 0 || tmp == 0) {
        cprintf(stdout, BOLD, "Invalid choice!\n");
        cprintf(stdout, BOLD, "Your choice: ");
        choice_str = read_line();
        errno = 0;
        tmp = strtol(choice_str, NULL, 10);
    }
    choice = (int) tmp;

    return choice;
}

/**
 * Menu system for registering this peer with the
 * manager.
 */
void getPeerInfo(char **name_ptr, char **ip_ptr, unsigned int *port_ptr)
{
    // Necessary pointer housekeeping
    if (!name_ptr || !ip_ptr || !port_ptr) {
        error("Cannot write to NULL!");
        exit(FAILURE);
    }

    char *prt;
    int status;
    struct sockaddr_in addr;

    cprintf(stdout, BOLD, " +-------------------+\n");
    cprintf(stdout, BOLD, " | Welcome to Bingo! |\n");
    cprintf(stdout, BOLD, " +-------------------+\n\n");
    cprintf(stdout, BOLD, "Please register.\n");

    // Name
    cprintf(stdout, BOLD, "Name: ");
    *name_ptr = read_line();

    // IP Address
    status = 0;
    while (status != 1) {
        cprintf(stdout, BOLD, "IP Address: ");
        *ip_ptr = read_line();
        status = inet_pton(AF_INET, *ip_ptr, &addr.sin_addr);
        if (status != 1) {
            cprintf(stdout, BOLD, "Invalid IP address!\n");
        }
    }

    // Port
    cprintf(stdout, BOLD, "Port: ");
    prt = read_line();
    errno = 0;
    long int tmp = strtol(prt, NULL, 10);
    while (errno != 0 || !(0 < tmp && tmp <= 65535)) {
        if (errno != 0) {
            errno = 0;
        } else {
            cprintf(stdout, BOLD, "Port must be between 1 and 65535.\n");
        }
        cprintf(stdout, BOLD, "Port: ");
        prt = read_line();
        tmp = strtol(prt, NULL, 10);
    }
    *port_ptr = (unsigned short) tmp;
}

/**
 * Main runtime of bingo application
 */
int main(int argc, char **argv)
{
    /****************
     * HOUSEKEEPING *
     ****************/

    // Arg checking.
    if (argc != 3) {
        cprintf(stdout, BOLD, "Usage: ");
        fprintf(stdout, "%s <server_ip> <port>\n", argv[0]);
        exit(FAILURE);
    }

    // Try to convert port.
    unsigned short port;
    errno = 0;
    long int tmp = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        error("Invalid port number.");
        exit(FAILURE);
    }
    if (0 <= tmp && tmp <= 65535) {
        port = (unsigned short) tmp;
    } else {
        error("Port must be between 0 and 65535.");
        exit(FAILURE);
    }

    /****************
     * REGISTRATION *
     ****************/
    char *p_name, *p_ip;
    unsigned int p_port;
    getPeerInfo(&p_name, &p_ip, &p_port);

    // Create socket for communication with manager
    bingo_sock = new ClientSocket(argv[1], port);

    // Establish connection with manager
    info("Establishing connection with manager...");
    bingo_sock->start();

    // Create player
    Player *newPlayer = new Player(p_name, p_ip, p_port);

    // Attempt to register player with manager
    newPlayer->registerToManager(bingo_sock);

    // Close connection with manager
    info("Closing connection with manager...");
    bingo_sock->stop();
    delete bingo_sock;
    bingo_sock = NULL;

    /********
     * MENU *
     ********/
    /*int choice = 0;
    printMenu();
    choice = getChoice();*/

    return SUCCESS;
}
