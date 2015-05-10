#include <memory>
#include <deque>

#include "game_objects.h"

class StrategyTask {
public:
    virtual Point getTargetPoint(const World& world, const Ball& ball){
        return ball.position;
    }

    virtual bool isActual(const World& world, const Ball& ball) {
        return false;
    }
};

typedef std::shared_ptr<StrategyTask> StrategyTaskPtr;

class GlobalStrategy {
private:
    int updateTime_;
    int timeWithoutUpdate_;
    std::deque<StrategyTaskPtr> cachedTasks_;

    void removeNonActualTasks(const World& world, const Ball& ball) {
        while (!cachedTasks_.empty()) {
            auto task = cachedTasks_.front();
            if (task->isActual(world, ball)) {
                break;
            }
            cachedTasks_.pop_front();
        }
    }

public:
    GlobalStrategy(int updateTime)
            : updateTime_(updateTime)
            , timeWithoutUpdate_(0) {}

    StrategyTaskPtr getTask(const World& world, const Ball& ball) {
        removeNonActualTasks(world, ball);
        if (timeWithoutUpdate_ == updateTime_ || cachedTasks_.empty()) {
            cachedTasks_ = estimateActions(world, ball);
            timeWithoutUpdate_ = 0;
        }
        if (cachedTasks_.empty()) {
            cachedTasks_.push_back(StrategyTaskPtr(new StrategyTask()));
        }
        auto result = cachedTasks_.front();
        return result;
    }

    virtual std::deque<StrategyTaskPtr> estimateActions(const World& world, const Ball& ball);
};

class MovementStrategy {
public:
    virtual Acceleration getAcceleration(const World& world,
                                  StrategyTaskPtr strategyTaskPtr, const Ball& ball);
};

