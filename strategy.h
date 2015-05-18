#include <memory>
#include <deque>
#include <numeric>
#include <limits>

#include "game_objects.h"
#include "utils.h"

class StrategyTask {
public:
    virtual Point getTargetPoint(const World& world, const Ball& ball){
        return ball.position_;
    }

    virtual bool isActual(const World& world, const Ball& ball) {
        return false;
    }

    virtual ~StrategyTask() {}
};

class TakeCoinTask : public StrategyTask {
private:
    Coin target_;

public:
    TakeCoinTask(const Coin& coin)
            : target_(coin)
    {
    }

    Point getTargetPoint(const World& world, const Ball& ball){
        return target_.position_;
    }

    bool isActual(const World& world, const Ball& ball) {
        for (auto coin : world.coins) {
            if (dist(target_.position_, coin.position_) < 1e-2) {
                return true;
            }
        }
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
            , timeWithoutUpdate_(0)
    {
    }

    virtual ~GlobalStrategy() {}

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
        ++timeWithoutUpdate_;
        return result;
    }

    virtual std::deque<StrategyTaskPtr> estimateActions(const World& world, const Ball& ball) {
        return std::deque<StrategyTaskPtr>();
    }
};

typedef std::function<double(const Ball&, const Coin&)> Estimator;

double naiveDistEstimator(const Ball& ball, const Coin& coin) {
    return dist(ball.position_, coin.position_);
}

Estimator createVelocityDistEstimator(double velocityCoeff) {
    return [&](const Ball& ball, const Coin& coin) {
        Point point(ball.position_.x_ + ball.velocity_.v_x_,
                    ball.position_.y_ + ball.velocity_.v_y_);

        return dist(ball.position_, coin.position_) +
                velocityCoeff * rotateCos(ball.position_, point, coin.position_);
    };
}

class NearestCoinStrategy : public GlobalStrategy {
private:
    Estimator estimator_;

public:
    NearestCoinStrategy(int updateTime, Estimator estimator = createVelocityDistEstimator(0))
        : GlobalStrategy(updateTime)
        , estimator_(estimator)
    {
    }

    std::deque<StrategyTaskPtr> estimateActions(const World& world, const Ball& ball) {
        std::deque<StrategyTaskPtr> result;
        double dst = std::numeric_limits<double>::max();
        int pos = -1;
        for (int i = 0; i < world.coins.size(); ++i) {
            double value = estimator_(ball, world.coins[i]);
            if (value < dst) {
                dst = value;
                pos = i;
            }
        }
        if (pos >= 0) {
            result.emplace_back(new TakeCoinTask(world.coins[pos]));
        }
        return result;
    }
};

class MovementStrategy {
public:
    virtual Acceleration getAcceleration(const World& world,
                                  StrategyTaskPtr strategyTaskPtr, const Ball& ball);
};

