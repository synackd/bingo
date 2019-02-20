#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "bingo.hpp"

using namespace std;

/*
 * Structures for messages
 */

/**
 * Used for CALLERACK, STARTGAME, NUMBERCALL,...
 */
typedef struct message {
    int commandCode;
    int parameters;
} message;

typedef struct startGameResponse {
    int gameID;
    int playersLeft;
    string playerIP;
    int playerPort;
} startGameResponse;

/*
 * Class Prototypes
 */

class Player {
    private:
        string name;
        string ip;
        int port;

    public:
        Player(string inputName, string inputIP, int inputPort);
        string to_string(void);
        string getName(void);
        string getIP(void);
        int getPort(void);
        void listenBingo(void);
        void registerToManager(void);
};

/**
 * Used in Caller role to keep track of players in each game
 */
class PlayerData {
    private:
        string name;
        string ip;
        int port;

    public:
        PlayerData(string inputName, string inputIP, int inputPort);
        string to_string(void);
        string getName(void);
        string getIP(void);
        int getPort(void);
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