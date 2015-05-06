#include <iostream>
#include <vector>

class Point
{
public:
    double x;
    double y;
};

class Ball
{
public:
    size_t ball_id;
    Point position;
    Point velocity;
};

class Turn
{
public:
    size_t ball_id;
    unsigned long long world_id;
    Point acceleration;
};

class World
{
public:
    unsigned long long world_id;
    double fieldRadius;
    double ballRadius;
    double coinRadius;

    std::vector<Ball> balls;
    std::vector<Point> coins;
};