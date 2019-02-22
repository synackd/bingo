#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "client.hpp"
#include "bingo.hpp"

using namespace std;

/*
 * Class Prototypes
 */

class Player {
    private:
        bool registered;
        string name;
        string ip;
        unsigned int port;
        ClientSocket *sock;

    public:
        Player(string inputName, string inputIP, unsigned int inputPort);
        string to_string(void);
        string getName(void);
        string getIP(void);
        bool isRegistered(void);
        unsigned int getPort(void);
        void listenBingo(void);
        int regist(ClientSocket *sock);
        int deregist(ClientSocket *sock);
};

/**
 * Used to keep track of games in Caller and Manager
 */
class Game {
    private:
        int id;
        int k;
        vector<Player> playersList;

    public:
        Game(int inputGameID, int inputK);
        void addPlayer(Player inputPlayer);
};

/*
 * Used to represent the Bing Board cell
 */
class Cell {
    private:
        int value;
        bool called;

    public:
        Cell(void);
        Cell(int inputValue);
        bool isCalled(void);
        void setCalled(bool value);
        int getValue(void);
};

/*
 * Represents the Bingoboard and contains methods
 */
class Board {
    private:
        Cell values[3][3];

    public:
        Board(void);
        void markNumber(int calledValue);
        bool checkWin(void);
        void printBoard(void);
        int getValue(int row, int column);
        void setCalled(int row, int column, bool value);
        bool isCalled(int row, int column);
};


class Caller{
    public:
        Caller(void);
        void call(int sockfd);
        void startGame(void);
        void printBoard(void);
};

#endif
