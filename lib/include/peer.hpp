#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include "client.hpp"
#include "player.hpp"

using namespace std;

/*
 * Class Prototypes
 */

/**
 * Represents a base class for Player and Caller
 */
class Peer
{
    protected:
        bool registered;
        string name;
        string ip;
        unsigned int port;

    public:
        Peer(string inputName, string inputIP, unsigned int inputPort);
        string to_string(void);
        string getName(void);
        string getIP(void);
        unsigned int getPort(void);
        bool isRegistered(void);
        int setPort(unsigned int newPort);
        int regist(ClientSocket *sock);
        int deregist(ClientSocket *sock);
};

/**
 * The player in the bingo game which listens for
 * numbers called by the Caller
 */
class Player: public Peer
{
    public:
        Player(string inputName, string inputIP, unsigned int inputPort): Peer(inputName, inputIP, inputPort) {}
        string to_string(void);
        void listenBingo(void);
};

/**
 * The caller of the bingo game which calls numbers
 * for the Players
 */
class Caller: public Peer
{
    public:
        vector<Player> players;

        Caller(string inputName, string inputIP, unsigned int inputPort): Peer(inputName, inputIP, inputPort) {}
        void call(int sockfd);
        void startGame(void);
        void printBoard(void);
};

/**
 * Used to keep track of games in Caller and Manager
 */
class Game {
    private:
        int id;
        int k;
        Player *gameCaller;
        vector<PlayerData> playersList;

    public:
        Game(int inputGameID, int inputK, Player *inputCaller);
        int getID();
        void addPlayer(PlayerData inputPlayer);
        void printGameData();
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
        Cell values[5][5];

    public:
        Board(void);
        bool checkUsedValue(int value, vector<int>list, int listSize);
        void markNumber(int calledValue);
        bool checkWin(void);
        void printBoard(void);
        void logBoard(FILE *fp);
        int getValue(int row, int column);
        void setCalled(int row, int column, bool value);
        bool isCalled(int row, int column);
};


#endif
