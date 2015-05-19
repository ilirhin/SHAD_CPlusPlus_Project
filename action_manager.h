#include "strategy.h"

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

    void performViewerAction(const World &world) {
        std::cout << "Viewer performs action" << std::endl;
        // ToDo
    }
};