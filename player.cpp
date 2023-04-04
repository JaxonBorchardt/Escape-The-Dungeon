#include "player.h"

#include <cmath>
#include <iostream>

#include "engine.h"
#include "world.h"

Player::Player(Engine &engine, const Vec<double> &position,
               const Vec<int> &size)
    : size{size}
{
    physics.position = position;
    state = std::make_unique<Standing>();
    state->enter(*this);

    standing = engine.graphics.get_animated_sprite("standing", 0.15, false, false);

    jumping = engine.graphics.get_animated_sprite("jumping", 0.15, false, false);
    running = engine.graphics.get_animated_sprite("running", 0.15, false, false);

    sprite = standing.get_sprite();
}

std::unique_ptr<Command> Player::handle_input(const SDL_Event &event)
{
    auto new_state = state->handle_input(*this, event);
    if (new_state)
    {
        state->exit(*this);
        state = std::move(new_state);
        state->enter(*this);
    }

    auto next = std::move(next_command);
    next_command = nullptr;
    return next;
    // if (event.type == SDL_KEYDOWN) {
    //     SDL_Keycode key = event.key.keysym.sym;
    //     if (key == SDLK_RIGHT) {
    //         // velocity.x = terminal_velocity;
    //         acceleration.x = walk_acceleration;
    //     } else if (key == SDLK_LEFT) {
    //         // velocity.x = terminal_velocity;
    //         acceleration.x = -walk_acceleration;
    //     } else if (key == SDLK_SPACE) {
    //         velocity.y = jump_velocity;
    //         acceleration.y = gravity;
    //     }
    // }
    // if (event.type == SDL_KEYUP) {
    //     SDL_Keycode key = event.key.keysym.sym;
    //     if (key == SDLK_RIGHT) {
    //         // velocity.x = 0;
    //         acceleration.x = 0;
    //     } else if (key == SDLK_LEFT) {
    //         // velocity.x = 0;
    //         acceleration.x = 0;
    //     }
    // }
}

void Player::update(Engine &engine, double dt)
{
    auto new_state = state->update(*this, engine, dt);
    if (new_state)
    {
        state->exit(*this);
        state = std::move(new_state);
        state->enter(*this);
    }
    if (next_command)
    {
        next_command->execute(*this, engine);
        next_command = nullptr;
    }
}

std::pair<Vec<double>, Color> Player::get_sprite() const
{
    return {physics.position, color};
}
