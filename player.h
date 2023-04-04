#pragma once

#include <SDL2/SDL.h>
#include <vec.h>

#include <memory>

#include "command.h"
#include "fsm.h"
#include "graphics.h"
#include "physics.h"
// forward declaration
class World;

class Player
{
public:
    Player(Engine &engine, const Vec<double> &position, const Vec<int> &size);
    std::unique_ptr<Command> handle_input(const SDL_Event &event);
    void update(Engine &engine, double dt);
    std::pair<Vec<double>, Color> get_sprite() const;

    Physics physics;
    const double walk_acceleration = 25;
    const double jump_velocity = 7;
    Vec<int> size;
    Color color{255, 0, 0, 255};

    Sprite sprite;
    AnimatedSprite standing;
    AnimatedSprite jumping;
    AnimatedSprite running;

    std::unique_ptr<State> state;
    std::unique_ptr<Command> next_command;
};
