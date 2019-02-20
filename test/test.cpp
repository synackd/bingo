#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include <iostream>

#include <vector>
#include "bingo.h"
using namespace std;

int main(int argc, char **argv)
{
    BingoBoard testBoard = BingoBoard();

    testBoard.PrintBoard();

    int value;
    bool gameover = false;

    while (!gameover){
        value = rand() % 10;
        cout << "calling " << value << "\n";
        testBoard.CheckNumber(value);
        gameover = testBoard.CheckWin();
    }

    cout << "GAMEOVER\n";
    testBoard.PrintBoard();


}
