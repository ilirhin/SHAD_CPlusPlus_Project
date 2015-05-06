#include <string>

#include "action_manager.h"


enum ClientType { GAMER, VIEWER, UNDEFINED_TYPE };

class Client
{
private:
    ActionManager actionManager_;
    ClientType type_;
    std::string id_;
public:
    // TODO
    // connection to server
    // converting between output format and out inner format
    // something else
};