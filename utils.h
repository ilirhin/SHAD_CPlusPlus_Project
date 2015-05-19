//
// Created by Tylorn on 18.05.2015.
//
#include <math.h>
#include "game_objects.h"

#ifndef SHAD_CPLUSPLUS_PROJECT_UTILS_H
#define SHAD_CPLUSPLUS_PROJECT_UTILS_H

double dist(const Point &first, const Point &second) {
    return sqrt((first.x_ - second.x_) * (first.x_ - second.x_) +
                (first.y_ - second.y_) * (first.y_ - second.y_));
}

double product(const Point &first, const Point &second) {
    return first.x_ * second.x_ + first.y_ * second.y_;
}

double rotateCos(const Point &first, const Point &second, const Point &third) {
    double fx = first.x_;
    double sx = second.x_;
    double fy = first.y_;
    double sy = second.y_;

    return (fx * sx + fy * sy) / sqrt(fx * fx + fy * fy) / sqrt(sx * sx + sy * sy);
}

#endif //SHAD_CPLUSPLUS_PROJECT_UTILS_H
