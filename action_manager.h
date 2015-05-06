#include "strategy.h"

class ActionManager
{
private:
    std::shared_ptr<GlobalStrategy> globalStrategyPtr;
    std::shared_ptr<MovementStrategy> movementStrategyPtr;

public:
    ActionManager(std::shared_ptr<GlobalStrategy> globalStrategy,
                  std::shared_ptr<MovementStrategy> movementStrategy)
            : globalStrategyPtr(globalStrategy)
            , movementStrategyPtr(movementStrategy) {}

    Point getAction(const World& world, const Ball& ball) {
        return movementStrategyPtr->
                getAcceleration(world, globalStrategyPtr->getTask(world, ball), ball);
    }
};