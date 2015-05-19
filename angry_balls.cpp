#include <iostream>
#include <string>

#include "message_builder.h"
#include "message_parser.h"
#include "strategy.h"
#include "client.h"

int main() {
    Gamer gamer = Gamer(ActionManager());
    gamer.run(8800);

    return 0;
}