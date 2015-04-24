class Point {
public:
    double x;
    double y;
};

class Ball {
public:
    Point position;
    Point velocity;
};

class World {
public:
    double fieldRadius;
    double ballRadius;
    double coinRadius;

    std::vector<Ball> balls;
    std::vector<Point> coins;
};

class StrategyTask {
public:
    virtual Point getTargetPoint(const World& world, const Ball& ball) {
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
    std::deque<StrategyTask> cachedTasks_;

    void removeNonActualTasks(const World& world, const Ball& ball) {
        while (!cachedTasks_.empty()) {
            auto task = cachedTasks_.front();
            if (task.isActual(world, ball)) {
                break;
            }
            cachedTasks_.pop_front();   
        }
    }

public:
    GlobalStrategy(int updateTime)
        : updateTime_(updateTime) 
        , timeWithoutUpdate_(0)
    {
    }

    StrategyTaskPtr getTask(const World& world, const Ball& ball) {
        removeNonActualTasks(world, ball);
        if (timeWithoutUpdate_ == updateTime_ || cachedTasks_.empty()) {
            cachedTasks_ = estimateActions(world);
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
    virtual Point getAcceleration(const World& world, 
                StrategyTaskPtr strategyTaskPtr, const Ball& ball);
};


class ActionManager {
private:
    std::shared_ptr<GlobalStrategy> globalStrategy_;
    std::shared_ptr<MovementStrategy> movementStrategy_;

public:

    ActionManager(std::shared_ptr<GlobalStrategy> globalStrategy, 
                    std::shared_ptr<MovementStrategy> movementStrategy) 
        : globalStrategy_(globalStrategy)
        , movementStrategy_(movementStrategy)
    {
    }

    Point getAction(const World& world, const Ball& ball) {
        return movementStrategy_->getAcceleration(world, globalStrategy_->getTask(), ball);
    }
};

class Client {
private:
   ActionManager actionManager_; 
public:
    // TODO
    // connection to server
    // converting between output format and out inner format
    // something else
};