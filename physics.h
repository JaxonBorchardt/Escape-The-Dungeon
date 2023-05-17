#pragma once
#include "vec.h"

constexpr double terminal_velocity = 10;
constexpr double gravity = -8;
constexpr double damping = 0.3;

class Physics
{
public:
    void update(double dt);
    Vec<double> position, velocity, acceleration{0, 0};
    void apply_friction(double friction);
};