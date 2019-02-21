#include <string>
#include "player.hpp"

using namespace std;

/**************
 * PlayerData *
 **************/

/**
 * Create player information for caller
 */
PlayerData::PlayerData(string inputName, string inputIP, int inputPort)
{
    this->name = inputName;
    this->ip = inputIP;
    this->port = inputPort;
}

/**
 * Getter for player's name
 */
string PlayerData::getName()
{
    return this->name;
}

/**
 * Getter for player's IP address
 */
string PlayerData::getIP()
{
    return this->ip;
}

/**
 * Getter for player's port
 */
int PlayerData::getPort()
{
    return this->port;
}
