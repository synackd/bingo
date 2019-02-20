#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

using namespace std;

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

#endif
