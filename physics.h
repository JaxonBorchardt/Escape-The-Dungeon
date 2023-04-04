#pragma once
#include "vec.h"

constexpr double terminal_velocity = 50;
constexpr double gravity = -9;
constexpr double damping = 0.95;

class Physics {
   public:
    void update(double dt);
    Vec<double> position, velocity, acceleration{0, gravity};
};