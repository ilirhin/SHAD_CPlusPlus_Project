#include <algorithm>
#include <memory>
#include <deque>
#include <numeric>
#include <limits>
#include <set>

#include "game_objects.h"
#include "utils.h"

#pragma once

class StrategyTask {
public:
    virtual Point getTargetPoint(const World &world, const Ball &ball) {
        return ball.position_;
    }

    virtual bool isActual(const World &world, const Ball &ball) {
        return false;
    }

    virtual ~StrategyTask() { }
};

class TakeCoinTask : public StrategyTask {
private:
    Coin target_;

public:
    TakeCoinTask(const Coin &coin)
            : target_(coin) {
    }

    Point getTargetPoint(const World &world, const Ball &ball) {
        return target_.position_;
    }

    bool isActual(const World &world, const Ball &ball) {
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

    void removeNonActualTasks(const World &world, const Ball &ball) {
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
            : updateTime_(updateTime), timeWithoutUpdate_(0) {
    }

    virtual ~GlobalStrategy() { }

    StrategyTaskPtr getTask(const World &world, const Ball &ball) {
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

    virtual std::deque<StrategyTaskPtr> estimateActions(const World &world, const Ball &ball) = 0;
};

typedef std::function<double(const World &, const Ball &, const Coin &)> Estimator;

double naiveDistEstimator(const World &, const Ball &ball, const Coin &coin) {
    return dist(ball.position_, coin.position_);
}

Estimator createVelocityDistEstimator(double velocityCoeff) {
    return [&](const World &, const Ball &ball, const Coin &coin) {
        Point point(ball.position_.x_ + ball.velocity_.v_x_,
                    ball.position_.y_ + ball.velocity_.v_y_);

        return dist(ball.position_, coin.position_) +
               velocityCoeff * rotateCos(ball.position_, point, coin.position_);
    };
}

Estimator createAreaDensityEstimator(double densityCoeff) {
    return [&](const World &world, const Ball &ball, const Coin &coin) {
        double ans = 0;
        for (const Ball& nBall : world.balls) {
            if ((nBall.position_.x_ - ball.position_.x_) * (nBall.position_.x_ - coin.position_.x_) < 0 &&
                        (nBall.position_.y_ - ball.position_.y_) * (nBall.position_.y_ - coin.position_.y_) < 0) {
                ans += 1;
            }
        }
        return ans * densityCoeff;
    };
}

template <typename KernelFunction>
Estimator createCoinDensityEstimator(double densityCoeff, KernelFunction kernel) {
    return [=](const World &world, const Ball &ball, const Coin &coin) {
        double ans = 0;
        for (const Ball& nBall : world.balls) {
            ans += kernel(dist(coin.position_, nBall.position_));
        }
        return ans * densityCoeff;
    };
}

Estimator createComboEstimator(Estimator first, Estimator second) {
    return [=](const World &world, const Ball &ball, const Coin &coin) {
        return first(world, ball, coin) + second(world, ball, coin);
    };
}

class NearestCoinStrategy : public GlobalStrategy {
private:
    Estimator estimator_;

public:
    NearestCoinStrategy(int updateTime, Estimator estimator = createVelocityDistEstimator(0))
            : GlobalStrategy(updateTime), estimator_(estimator) {
    }

    std::deque<StrategyTaskPtr> estimateActions(const World &world, const Ball &ball) {
        std::deque<StrategyTaskPtr> result;
        double dst = std::numeric_limits<double>::max();
        int pos = -1;
        for (int i = 0; i < world.coins.size(); ++i) {
            double value = estimator_(world, ball, world.coins[i]);
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

class KNearestCoinsStrategy : public GlobalStrategy {
private:
    int kValue_;

    std::vector<std::vector<double>> buildDistances(const World &world) {
        std::vector<std::vector<double>> dists(world.coins.size());
        for (size_t i = 0; i < dists.size(); ++i) {
            for (size_t j = 0; j < dists.size(); ++j) {
                dists[i].push_back(dist(world.coins[i].position_, world.coins[j].position_));
            }
        }
        return dists;
    }

    std::vector<std::vector<int>> sortedNeighboures(const std::vector<std::vector<double>> &dists) {
        std::vector<std::vector<int>> nearests(dists.size());
        for (size_t i = 0; i < dists.size(); ++i) {
            nearests[i].assign(dists.size(), 0);
            std::iota(nearests[i].begin(), nearests[i].end(), 0);
            auto comparator = [&](int first, int second) {
                return dists[i][first] < dists[i][second] ||
                       dists[i][first] == dists[i][second] && first < second;
            };
            std::sort(nearests[i].begin(), nearests[i].end(), comparator);
        }
        return nearests;
    }

public:

    KNearestCoinsStrategy(int updateTime, int kValue)
            : GlobalStrategy(updateTime), kValue_(kValue) {
    }

    std::deque<StrategyTaskPtr> estimateActions(const World &world, const Ball &ball) {
        std::deque<StrategyTaskPtr> result;
        std::vector<std::vector<double>> dists = buildDistances(world);
        int coinsAmount = world.coins.size();
        std::vector<std::vector<int>> nearests = sortedNeighboures(dists);

        double bestLen = std::numeric_limits<double>::max();
        std::vector<int> bestRoute;

        for (int start = 0; start < dists.size(); ++start) {
            std::vector<char> usedInRoute(dists.size());
            std::vector<int> nextCandidates(dists.size());

            int curr = start;
            double len = dist(ball.position_, world.coins[start].position_);
            usedInRoute[start] = true;
            std::vector<int> route({start});

            for (int iter = 1; iter < kValue_; ++iter) {
                while (nextCandidates[curr] < coinsAmount && usedInRoute[nearests[curr][nextCandidates[curr]]]) {
                    ++nextCandidates[curr];
                }
                if (nextCandidates[curr] == coinsAmount) {
                    break;
                }
                int newCurr = nearests[curr][nextCandidates[curr]];
                len += dist(world.coins[curr].position_, world.coins[newCurr].position_);
                curr = newCurr;
                usedInRoute[curr] = true;
                route.push_back(curr);
            }
            if (len < bestLen) {
                bestLen = len;
                bestRoute = route;
            }
        }


        for (int pos : bestRoute) {
            result.emplace_back(new TakeCoinTask(world.coins[pos]));
        }
        return result;
    }
};

class MovementStrategy {
public:
    virtual Acceleration getAcceleration(const World &world,
                                         StrategyTaskPtr strategyTaskPtr, const Ball &ball) = 0;

    virtual ~MovementStrategy() {}
};

class FirstMovementStrategyImpl : public MovementStrategy {
    Acceleration getAcceleration(const World &world,
                                 StrategyTaskPtr strategyTaskPtr, const Ball &ball) {
        Point targerPoint = strategyTaskPtr->getTargetPoint(world, ball);
        std::cerr << "Target: " << targerPoint.x_ << " " << targerPoint.y_ << std::endl;
        Velocity currentVelocity = ball.velocity_;
        Point currentPosition = ball.position_;

        double accelerationX = targerPoint.x_ - currentPosition.x_ + currentVelocity.x_;
        double accelerationY = targerPoint.y_ - currentPosition.y_ + currentVelocity.y_;

        double length = getNorm(Point(accelerationX, accelerationY)) + 1e-4;

        return Acceleration(accelerationX / length, accelerationY / length);
    }
};

class SecondMovementStrategyImpl : public MovementStrategy {
    Acceleration getAcceleration(const World &world,
                                 StrategyTaskPtr strategyTaskPtr, const Ball &ball) {
        Point targerPoint = strategyTaskPtr->getTargetPoint(world, ball);
        Velocity currentVelocity = ball.velocity_;
        Point currentPosition = ball.position_;
        std::cerr << "Target: " << targerPoint.x_ << " " << targerPoint.y_ << std::endl;
        
        Point targetRelative(targerPoint.x_ - currentPosition.x_, targerPoint.y_ - currentPosition.y_);
        Point targetRelaviteNorm(targetRelative.x_ / (getNorm(targetRelative) + 1e-4), targetRelative.y_ / (getNorm(targetRelative) + 1e-4));

        double proection = scalarMult(Point(currentVelocity.v_x_, currentVelocity.v_y_), targetRelative);

        Point perpendicular(-targetRelative.y_, targetRelative.x_);
        Point perpNorm(-targetRelative.y_ / getNorm(perpendicular), targetRelative.x_ / getNorm(perpendicular));

        double perpProection = scalarMult(Point(currentVelocity.v_x_, currentVelocity.v_y_), perpNorm);

        Point accByPerp(-perpProection * perpNorm.x_, -perpProection * perpNorm.y_);

        double restAcc = 1 - std::min(1.0, getNorm(accByPerp));

        Point accToTarget(restAcc * targetRelaviteNorm.x_, restAcc * targetRelaviteNorm.y_);

        return Acceleration(accByPerp.x_ + accToTarget.x_, accByPerp.y_ + accToTarget.y_);
    }
};

class RandomMovementStrategyImpl : public MovementStrategy {
    Acceleration getAcceleration(const World &world,
                                 StrategyTaskPtr strategyTaskPtr, const Ball &ball) {
        double a_x = rand() - RAND_MAX / 2;
        double b_x = rand() - RAND_MAX / 2;
        double length = a_x * a_x + b_x * b_x + 1e-4;
        return Acceleration(a_x / length, b_x / length);
    }
};

