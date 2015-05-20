#include <iostream>
#include <string>

#include "message_builder.h"
#include "message_parser.h"
#include "strategy.h"
#include "client.h"

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int port = std::atoi(argv[1]);
    Gamer gamer = Gamer(ActionManager());
    gamer.run(port);
    return 0;
}