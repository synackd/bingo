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
        unsigned int port;

    public:
        PlayerData(string inputName, string inputIP, int inputPort);
        string to_string(void);
        string getName(void);
        string getIP(void);
        unsigned int getPort(void);
        void setName(string newName){
            name = newName;
        }

        void setIP(string newIP){
            ip = newIP;
        }

        void setPort(unsigned int newPort){
            port = newPort;
        }
};




#endif
