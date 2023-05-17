#pragma once

#include <SDL2/SDL.h>
#include <vec.h>

#include <memory>

#include "command.h"
#include "fsm.h"
#include "graphics.h"
#include "object.h"
#include "physics.h"
#include "projectile.h"
// forward declaration
class World;

class Player : public Object
{
public:
    Player(Engine &engine, const Vec<double> &position, const Vec<int> &size);
    std::unique_ptr<Command> handle_input(const SDL_Event &event);
    void update(Engine &engine, double dt);
    std::pair<Vec<double>, Color> get_sprite() const;

    const double walk_acceleration = 8;
    const double jump_velocity = 7;
    Vec<int> size;
    Color color{255, 0, 0, 255};

    AnimatedSprite standing;
    AnimatedSprite jumping;
    AnimatedSprite running;
    AnimatedSprite attacking;

    AnimatedSprite fox_standing;
    AnimatedSprite fox_running;
    AnimatedSprite fox_jumping;
    AnimatedSprite fox_attacking;
    AnimatedSprite fox_hurting;

    std::unique_ptr<State> state;
    std::unique_ptr<Command> next_command;

      Projectile ninja_star;
};
