#include <iostream>
#include <string>
#include <stdlib.h>

#include "message_builder.h"
#include "message_parser.h"
#include "options.h"


int main(int argc, char *argv[]) {
	/*
    Gamer gamer = Gamer(ActionManager(std::make_shared<GlobalStrategy>(new NearestCoinStrategy(1)),  
    								  std::make_shared<MovementStrategy>(new FirstMovementStrategyImpl()) ));
    gamer.run(100);
    */

    Options options(argc, argv);

    Gamer gamer = Gamer(ActionManager(options.GetGlobalStrategy(), options.GetMovementStrategy()));
    gamer.run(options.GetPort());

    return 0;
}