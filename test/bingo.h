#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <vector>
#include <iostream>

#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define STARTGAME 10
#define CALLERACK 1
#define BINGOCALL 2
#define PLAYERACK 3
#define GAMEOVER 4

using namespace std;

class Player{
public:
    string playerIP;
    int Port;

    Player(string inputIP, int inputPort){
        playerIP = inputIP;
        Port = inputPort;
    }

    void PrintPlayer(){
        cout << "Player IP: " << playerIP << "\t Port: " << Port << "\n";
    }

};

// Used to keep track of games in Caller and Manager:
class Game{
public:
    int GameID;
    int K;
    vector<Player>PlayersList;

    Game(int inputGameID, int inputK){
        GameID = inputGameID;
        K = inputK;
    }

    void AddPlayer(Player inputPlayer){
        PlayersList.push_back(inputPlayer);
    }

};


class BingoPair{
public:

    int value;
    bool called;

    BingoPair(){
        value = 0;
        called = false;
    }

    BingoPair(int inputValue){
        value = inputValue;
        called = false;
    }

};

class BingoBoard{
public:

    BingoPair values[3][3];

    BingoBoard(){
        for (int row = 0; row < 3; row++){
            for (int column = 0; column < 3; column++){
                values[row][column] = BingoPair(row*3 + column);
            }
        }
    }

    bool CheckNumber(int
    ){
        for (int row = 0; row < 3; row++){
            for (int column = 0; column < 3; column++){
                if (values[row][column].value == calledValue){
                    values[row][column].called = true;
                }
            }
        }
    }

    bool CheckWin(){
        bool winnerRow, winnerColumn;
        // Checking rows:
        for (int row = 0; row < 3; row++){
            winnerRow = true;   // Assuming row is completed
            for (int column = 0; column < 3; column++){
                // Looking for not called number
                if (values[row][column].called == false){
                    // cout << "row " << row << " not completed\n";
                    winnerRow = false;
                }
            }
            if (winnerRow){
                return true;    // WIN!
            }
        }

        // Checking columns:
        for (int column = 0; column < 3; column++){
            winnerColumn = true;   // Assuming row is completed
            for (int row = 0; row < 3; row++){
                // Looking for not called number
                if (values[row][column].called == false){
                    // cout << "column " << row << " not completed\n";
                    winnerColumn = false;
                }
            }
            if (winnerColumn){
                return true;    // WIN!
            }
        }

        return false;

    }

    void PrintBoard(){
        cout << "Value matrix:\n";
        for (int row = 0; row < 3; row++){
            for (int column = 0; column < 3; column++){
                cout << values[row][column].value << "\t";
            }
            cout << "\n";
        }

        cout << "Called matrix:\n";
        for (int row = 0; row < 3; row++){
            for (int column = 0; column < 3; column++){
                cout << values[row][column].called << "\t";
            }
            cout << "\n";
        }
    }

};

// Used for CALLERACK, STARTGAME, NUMBERCALL,...
struct message{
    int commandCode;
    int parameters;
};


struct startGameResponse{
    int gameID;
    int playersLeft;
    string playerIP;
    int playerPort;
};
