#include "strategy.h"
// #include "viewer.h"
#pragma once

class ActionManager {
private:
    std::shared_ptr<GlobalStrategy> globalStrategyPtr;
    std::shared_ptr<MovementStrategy> movementStrategyPtr;

public:
    ActionManager() { }

    ActionManager(std::shared_ptr<GlobalStrategy> globalStrategy,
                  std::shared_ptr<MovementStrategy> movementStrategy)
            : globalStrategyPtr(globalStrategy), movementStrategyPtr(movementStrategy) { }

    Acceleration performGamerAction(const World &world, const Ball &ball) {
        std::cout << "Gamer performs action" << std::endl;
        return movementStrategyPtr->
                getAcceleration(world, globalStrategyPtr->getTask(world, ball), ball);
    }
/*
    void performViewerAction(const World &world, Notifier* notifier, bool& show_first_time) {
        //std::cout << "Viewer performs action" << std::endl;
        if (show_first_time) {
            show_first_time = false;
            notifier->startShowing(world);
        } else {
            notifier->updateWorld(world);
        }
    }
*/
};
