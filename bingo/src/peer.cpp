#include "log.hpp"
#include "peer.hpp"
#include "cmd.hpp"

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

/**
 * Check board if there is a bingo
 */
bool Board::checkWin()
{
    bool winnerRow, winnerColumn;

    // Checking rows:
    for (int row = 0; row < 3; row++){
        winnerRow = true;   // Assuming row is completed
        for (int column = 0; column < 3; column++){
            // Looking for not called number
            if (values[row][column].isCalled() == false){
                // info("Row %d not completed.", row);
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
Player::Player(string inputName, string inputIP, unsigned int inputPort)
{
    this->name = inputName;
    this->ip = inputIP;
    this->port = inputPort;
    this->registered = false;
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
unsigned int Player::getPort()
{
    return this->port;
}

/**
 * Player listens for Numbers called by Caller
 */
// TODO: Make this object oriented using ClientSocket...
void Player::listenBingo()
{
}

/**
 * Registers to Manager for future games
 */
void Player::regist(ClientSocket *sock)
{
    ssize_t size = 0;

    // Create register packet
    msg_t reg_cmd;
    reg_cmd.command = REGISTER;
    strncpy(reg_cmd.mgr_cmd_register.name, this->name.c_str(), BUFMAX);
    strncpy(reg_cmd.mgr_cmd_register.ip, this->ip.c_str(), BUFMAX);
    reg_cmd.mgr_cmd_register.port = this->port;

    // Send packet to manager
    info("Attempting to register player \"%s\"...", this->name.c_str());
    size = sock->send((void*) &reg_cmd, sizeof(msg_t));
    info("Sent %d bytes to manager.", size);

    // Listen for manager's response
    msg_t reg_rsp;
    size = sock->receive((void*) &reg_rsp, sizeof(msg_t));
    info("Received %d bytes from manager.", size);

    // Examine return code
    if (reg_rsp.mgr_rsp_register.ret_code == SUCCESS) {
        info("Registration success!");
        this->registered = true;
    } else if (reg_rsp.mgr_rsp_register.ret_code == FAILURE) {
        error("Registration failed!");
    } else {
        error("Manager returned unknown value.");
    }
}

/**
 * Deregister player from manager
 */
void Player::deregist(ClientSocket *sock)
{
    ssize_t size = 0;

    // Create deregister packet
    msg_t dereg_cmd;
    dereg_cmd.command = DEREGISTER;
    strncpy(dereg_cmd.mgr_cmd_deregister.name, this->name.c_str(), BUFMAX);

    // Send packet to manager
    info("Attempting to deregister player \"%s\"...", this->name.c_str());
    size = sock->send((void*) &dereg_cmd, sizeof(msg_t));
    info("Sent %d bytes to manager.", size);

    // Listen for manager's response
    msg_t dereg_rsp;
    size = sock->receive((void*) &dereg_rsp, sizeof(msg_t));
    info("Received %d bytes from manager.", size);

    // Examine return code
    if (dereg_rsp.mgr_rsp_deregister.ret_code == SUCCESS) {
        info("Deregistration success!");
        this->registered = false;
    } else if (dereg_rsp.mgr_rsp_deregister.ret_code == FAILURE) {
        error("Deregistration failed!");
    } else {
        error("Manager returned unknown value.");
    }
}

/**
 * Check if a player is registered
 */
bool Player::isRegistered()
{
    return this->registered;
}
