#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <iostream>
#include <vector>

class Point
{
public:
    double x_;
    double y_;

    Point() {}

    Point(double x, double y) : x_(x), y_(y) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.String("x");
        writer.Double(x_);
        writer.String("y");
        writer.Double(y_);
    }
};

class Velocity
{
public:
    double v_x_;
    double v_y_;

    Velocity() {}

    Velocity(double v_x, double v_y) : v_x_(v_x), v_y_(v_y) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.String("v_x");
        writer.Double(v_x_);
        writer.String("v_y");
        writer.Double(v_y_);
    }
};

class Acceleration
{
public:
    double a_x_;
    double a_y_;

    Acceleration() {}

    Acceleration(double a_x, double a_y) : a_x_(a_x), a_y_(a_y) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.String("a_x");
        writer.Double(a_x_);
        writer.String("a_y");
        writer.Double(a_y_);
    }
};

class Ball
{
public:
    size_t id_;
    Point position_;
    Velocity velocity_;
    double score_;

    Ball(size_t id, Point position, Velocity velocity, double score) :
            id_(id), position_(position), velocity_(velocity), score_(score) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();
        writer.String("id");
        writer.Uint(id_);
        position_.Serialize(writer);
        velocity_.Serialize(writer);
        writer.String("score");
        writer.Double(score_);
        writer.EndObject();
    }
};

class Coin
{
public:
    Point position_;
    double value_;

    Coin(Point position, double value) : position_(position), value_(value) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.StartObject();
        position_.Serialize(writer);
        writer.String("value");
        writer.Double(value_);
        writer.EndObject();
    }
};

class Turn
{
public:
    unsigned long long world_id_;
    size_t ball_id_;
    Acceleration acceleration_;

    Turn() {}

    Turn(unsigned long long world_id, size_t ball_id, Acceleration acceleration) :
            world_id_(world_id), ball_id_(ball_id), acceleration_(acceleration) {}

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.String("state_id");
        writer.Uint64(world_id_);
        writer.String("id");
        writer.Uint(ball_id_);
        acceleration_.Serialize(writer);
    }

};

class World
{
public:
    unsigned long long world_id; // id of state of the world
    double field_radius;
    double ball_radius;
    double coin_radius;

    double delta_time;
    double max_velocity;

    std::vector<Ball> balls;
    std::vector<Coin> coins;

    template <typename Writer>
    void Serialize(Writer& writer) const {
        writer.String("state_id");
        writer.Uint64(world_id);
        writer.String("field_radius");
        writer.Double(field_radius);
        writer.String("player_radius");
        writer.Double(ball_radius);
        writer.String("coin_radius");
        writer.Double(coin_radius);
        writer.String("time_delta");
        writer.Double(delta_time);
        writer.String("velocity_max");
        writer.Double(max_velocity);
        writer.String("players");
        writer.StartArray();
        for (const Ball& ball : balls) {
            ball.Serialize(writer);
        }
        writer.EndArray();
        writer.String("coins");
        writer.StartArray();
        for (const Coin& coin : coins) {
            coin.Serialize(writer);
        }
        writer.EndArray();
    }
};

#endif