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

// CLASSES: /////////////////////////////////////////

// Used in Caller role to keep track of players in each game:
class PlayerData{
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

// Used to represent the Bing Board cell:
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

// Represents the Bingoboard and contains methods:
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

    bool CheckNumber(int calledValue){
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


class Caller{
public:
    // Calls numbers to players:
    void CallBingo(int sockfd){
        ssize_t n;
        int inputCode;
    	bool gameOver = false;
    	int value;

        message callMessage; 	// message for sending
    	message playerResponse;		// message to receive ACK from player

    	while (!gameOver){
    		// Populating callMessage:
    		callMessage.commandCode = BINGOCALL;
    		value = rand() % 10;
    		callMessage.parameters = value;

    		// Sending 'Start game K' command:
    		cout << "calling " << value << "\n";
    		n = write(sockfd, &callMessage, sizeof(message));
    		if (n < 0)
    			DieWithError("ERROR writing to socket");

    		n = read(sockfd, &playerResponse, sizeof(startGameResponse));
    		if (n < 0)
    			DieWithError("ERROR reading from socket");
    		else{

    			if (playerResponse.commandCode == PLAYERACK)
    				cout << "Player ACK received.\n";

    			if (playerResponse.commandCode == GAMEOVER)
    				gameOVer = true;
    		}

    	} // end of while loop

    	cout << "Game Over! \n";
    }

    // Request players to manager:
    void StartGame(){

    }

};


class Manager{
public:
    // Sends K players to Caller that requested them:
    void SendKPlayers(){

    }

    // Registers players who want to be considered to play Bingo:
    void RegisterPlayer(){

    }

};


class Player{
public:

    // Player listens for Numbers called by Caller:
    void ListenBingo(){
        ssize_t n;
        int inputCode;
        int calledNumber;
    	bool gameOver = false;

        message inputMessage;
    	message callerACK;
    	callerACK.commandCode = PLAYERACK;

        for ( ; ; ) {
    	    if ( (n = read(sockfd, &inputMessage, sizeof(message))) == 0 )
       	    	return; /* connection closed by other end */

            inputCode = inputMessage.commandCode;

            if (inputCode == BINGOCALL){
                printf("Bingo Call received!\n");
    			calledNumber = inputMessage.parameters;
                cout << "Number: " << calledNumber << "\n";

    			gameBoard.CheckNumber(calledNumber);
    	        gameOver = gameBoard.CheckWin();

    			// updating command code if player wins:
    			if (gameOver){
    				callerACK.commandCode = GAMEOVER;

    				gameBoard.PrintBoard();
    			}

    			n = write(sockfd, &callerACK, sizeof(message));
    			if (n < 0)
    				DieWithError("ERROR writing to socket");
    			cout << "ACK/GAMEOVER sent to caller.\n";

            }

        }// end of for loop
    }

    // Registers to Manager for future games:
    void RegisterToManager(){

    }
}



// STRUCTS FOR MESSAGES ////////////////////////////////////
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
