//
// Created by Tylorn on 18.05.2015.
//
#include <math.h>
#include <cmath>
#include "game_objects.h"

#ifndef SHAD_CPLUSPLUS_PROJECT_UTILS_H
#define SHAD_CPLUSPLUS_PROJECT_UTILS_H

double getNorm(const Point &vec) {
    return sqrt(vec.x_ * vec.x_ + vec.y_ * vec.y_);
}

double dist(const Point &first, const Point &second) {
    return sqrt((first.x_ - second.x_) * (first.x_ - second.x_) +
                (first.y_ - second.y_) * (first.y_ - second.y_));
}

double product(const Point &first, const Point &second) {
    return first.x_ * second.x_ + first.y_ * second.y_;
}

double rotateCos(const Point &first, const Point &second, const Point &third) {
    double fx = second.x_ - first.x_;
    double sx = third.x_ - second.x_;
    double fy = second.y_ - first.y_;
    double sy = third.y_ - second.y_;

    return (fx * sx + fy * sy) / sqrt(fx * fx + fy * fy) / sqrt(sx * sx + sy * sy);
}

double cosBetweenVectors(const Point &first, const Point &second) {
    return product(first, second) / getNorm(first) / getNorm(second);
}

double getAngleToOX(const Point &current, const Point &target) {
    double ox = current.x_;
    double oy = target.y_;

    double ax = current.x_ - target.x_;
    double ay = current.y_ - target.y_;

    double bx = ox - target.x_;
    double by = oy - target.y_;

    double x = target.x_ - ox;
    double y = target.y_ - oy;

    if ((ax * by - ay * bx) > 0) {
        return std::acos(x / (x * x + y * y));
    } else {
        return std::acos(-1) - std::acos(x / (x * x + y * y));
    }
}

Velocity rotateAndMove(const Velocity &point, const Point &moveVector, double angle) {
    double newX = -point.v_y_ * std::sin(angle) + point.v_y_ * std::cos(angle) - moveVector.x_;
    double newY =  point.v_x_ * std::sin(angle) + point.v_y_ * std::cos(angle) - moveVector.y_;

    return Velocity(newX, newY);
}

#endif //SHAD_CPLUSPLUS_PROJECT_UTILS_H

