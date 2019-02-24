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
Game::Game(int inputGameID, int inputK, Player *inputCaller)
{
    this->id = inputGameID;
    this->k = inputK;
    this->gameCaller = inputCaller;
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

 bool Board::checkUsedValue(int value, vector<int>list, int listSize){
     for (int i = 0; i < listSize; i ++){
         if (value == list[i])
             return true;
     }
     return false;
 }

/**
 * Create a new bingo board
 */
Board::Board()
{
    int value;
    vector<int>usedNumbers;
    int usedNumbersCount = 0;


    for (int column = 0; column < 5; column++){
        for (int row = 0; row < 5; row++){

            value = (rand() % 15) + 15*column;
            while (checkUsedValue(value, usedNumbers, usedNumbersCount)){
                value = (rand() % 15) + 15*column;
            }
            this->values[row][column] = value;

            usedNumbers.push_back(value);
            usedNumbersCount++;
        }
    }
}

/**
 * Mark number on board
 */
void Board::markNumber(int calledValue)
{
    for (int row = 0; row < 5; row++){
        for (int column = 0; column < 5; column++){
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
    for (int row = 0; row < 5; row++){
        winnerRow = true;   // Assuming row is completed
        for (int column = 0; column < 5; column++){
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
    info("|| BINGO BOARD ||");
    for (int row = 0; row < 5; row++){
        for (int column = 0; column < 5; column++){
            cout << to_string(this->getValue(row, column)) << "\t";
        }
        cout << "\n";
    }
}

/**********
 * Caller *
 **********/

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

/********
 * Peer *
 ********/

/**
 * Store peer's information
 */
Peer::Peer(string inputName, string inputIP, unsigned int inputPort)
{
    this->name = inputName;
    this->ip = inputIP;
    this->port = inputPort;
    this->registered = false;
}

/**
 * Return string of peer's information
 */
string Peer::to_string()
{
    string str = "Peer Name: ";
    str += this->name;
    str += "\tPlayer IP: ";
    str += this->ip;
    str += "\tPort: ";
    str += std::to_string(this->port);
    return str;
}

/**
 * Getter for peer's name
 */
string Peer::getName()
{
    return this->name;
}

/**
 * Getter for peer's IP address
 */
string Peer::getIP()
{
    return this->ip;
}

/**
 * Getter for peer's port
 */
unsigned int Peer::getPort()
{
    return this->port;
}

/**
 * Setter for peer's port
 */
int Peer::setPort(unsigned int newPort)
{
    if (1 <= newPort && newPort <= 65535) {
        this->port = newPort;
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

/**
 * Registers to Manager for future games
 */
int Peer::regist(ClientSocket *sock)
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
        return SUCCESS;
    } else if (reg_rsp.mgr_rsp_register.ret_code == FAILURE) {
        error("Registration failed!");
        return FAILURE;
    } else {
        error("Manager returned unknown value.");
        return FAILURE;
    }
}

/**
 * Deregister peer from manager
 */
int Peer::deregist(ClientSocket *sock)
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
        return SUCCESS;
    } else if (dereg_rsp.mgr_rsp_deregister.ret_code == FAILURE) {
        error("Deregistration failed!");
        return FAILURE;
    } else {
        error("Manager returned unknown value.");
        return FAILURE;
    }
}

/**
 * Check if a peer is registered
 */
bool Peer::isRegistered()
{
    return this->registered;
}

/**********
 * Player *
 **********/

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
 * Player listens for Numbers called by Caller
 */
// TODO: Make this object oriented using ClientSocket...
void Player::listenBingo()
{
}
