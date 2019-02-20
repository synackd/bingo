#include "peer.hpp"

using namespace std;

/********
 * Game *
 ********/

/**
 * Create a game for the caller
 */
Game::Game(int inputGameID, int inputK)
{
    this->id = inputGameID;
    this->k = inputK;
}

/**
 * Add a player to the game
 */
void Game::addPlayer(Player inputPlayer)
{
    this->playersList.push_back(inputPlayer);
}

/********
 * Cell *
 ********/

/**
 * Initialize an empty bingo square
 */
Cell::Cell()
{
    this->value = 0;
    this->called = false;
}

/**
 * Initialize a non-empty bingo square
 */
Cell::Cell(int inputValue)
{
    this->value = inputValue;
    this->called = false;
}

/**
 * Return if cell has been called
 */
bool Cell::isCalled()
{
    return this->called;
}

/**
 * Set called value of cell
 */
void Cell::setCalled(bool value)
{
    this->called = value;
}

/**
 * Return the value of the cell
 */
int Cell::getValue()
{
    return this->value;
}

/*********
 * Board *
 *********/

/**
 * Create a new bingo board
 */
Board::Board()
{
    for (int row = 0; row < 3; row++){
        for (int column = 0; column < 3; column++){
            this->values[row][column] = Cell(row*3 + column);
        }
    }
}

/**
 * Mark number on board
 */
void Board::markNumber(int calledValue)
{
    for (int row = 0; row < 3; row++){
        for (int column = 0; column < 3; column++){
            if (this->getValue(row, column) == calledValue){
                this->setCalled(row, column, true);
            }
        }
    }
}

/**
 * Return the value of board's cell
 */
int Board::getValue(int row, int column)
{
    return this->values[row][column].getValue();
}

/**
 * Check if a cell has been called
 */
bool Board::isCalled(int row, int column)
{
    return this->values[row][column].isCalled();
}

/**
 * Set a certain cell's called value
 */
void Board::setCalled(int row, int column, bool value)
{
    this->values[row][column].setCalled(value);
}

bool Board::checkWin()
{
    bool winnerRow, winnerColumn;

    // Checking rows:
    for (int row = 0; row < 3; row++){
        winnerRow = true;   // Assuming row is completed
        for (int column = 0; column < 3; column++){
            // Looking for not called number
            if (values[row][column].isCalled() == false){
                // cout << "row " << row << " not completed\n";
                winnerRow = false;
            }
        }
        if (winnerRow){
            return true;    // WIN!
        }
    }

    return false;
}

/**
 * Print the bingo board to stdout
 */
void Board::printBoard()
{
    info("Value matrix:");
    for (int row = 0; row < 3; row++){
        for (int column = 0; column < 3; column++){
            cout << to_string(this->getValue(row, column)) << "\t";
        }
        cout << "\n";
    }

    info("Called matrix:");
    for (int row = 0; row < 3; row++){
        for (int column = 0; column < 3; column++){
            cout << this->isCalled(row, column) << "\t";
        }
        cout << "\n";
    }
}

/**********
 * Caller *
 **********/

/**
 * Create a new caller
 */
Caller::Caller()
{
}

/**
 * Calls numbers to players
 */
// TODO: Make this object oriented with ServerSocket...
void Caller::call(int sockfd)
{
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
        info("Calling %d", value);
        n = write(sockfd, &callMessage, sizeof(message));
        if (n < 0) {
            error("Could not write to socket!");
            // TODO: Implement return code here
            return;
        }

        n = read(sockfd, &playerResponse, sizeof(startGameResponse));
        if (n < 0) {
            error("Could not read from socket!");
            // TODO: Implement return code here
            return;
        } else {

            if (playerResponse.commandCode == PLAYERACK)
                info("Player ACK received.");

            if (playerResponse.commandCode == GAMEOVER)
                gameOver = true;
        }

    } // end of while loop

    info("Game Over!");
}

/**
 * Request players to manager
 */
void Caller::startGame()
{
}

/**********
 * Player *
 **********/

/**
 * Store player's information
 */
Player::Player(string inputName, string inputIP, int inputPort)
{
    this->name = inputName;
    this->ip = inputIP;
    this->port = inputPort;
}

/**
 * Return string of player's information
 */
string Player::to_string()
{
    string str = "Player Name: ";
    str += this->name;
    str += "\tPlayer IP: ";
    str += this->ip;
    str += "\tPort: ";
    str += std::to_string(this->port);
    return str;
}

/**
 * Getter for player's name
 */
string Player::getName()
{
    return this->name;
}

/**
 * Getter for player's IP address
 */
string Player::getIP()
{
    return this->ip;
}

/**
 * Getter for player's port
 */
int Player::getPort()
{
    return this->port;
}
/**
 * Player listens for Numbers called by Caller
 */
// TODO: Make this object oriented using ClientSocket...
void Player::listenBingo()
{
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
            info("Bingo Call received!");
            calledNumber = inputMessage.parameters;
            info("Number: ", calledNumber);

            gameBoard.CheckNumber(calledNumber);
            gameOver = gameBoard.CheckWin();

            // updating command code if player wins:
            if (gameOver){
                callerACK.commandCode = GAMEOVER;
                gameBoard.PrintBoard();
            }

            n = write(sockfd, &callerACK, sizeof(message));
            if (n < 0) {
                error("Could not write to socket!");
                // TODO: Implement return code here
                return;
            }
            info("ACK/GAMEOVER sent to caller.");
        }
    }// end of for loop
}

/**
 * Registers to Manager for future games
 */
void Player::registerToManager(){
}
