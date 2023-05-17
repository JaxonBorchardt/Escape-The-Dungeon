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
    physics.acceleration.y = gravity;
    combat.health = 5;
    combat.max_health = 5;
    combat.attack_damage = 3;
    state = std::make_unique<Standing>();
    state->enter(*this);

    standing = engine.graphics.get_animated_sprite("standing", 0.35, false, false);
    jumping = engine.graphics.get_animated_sprite("jumping", 0.25, false, false);
    running = engine.graphics.get_animated_sprite("running", 0.15, false, false);
    attacking = engine.graphics.get_animated_sprite("attacking", 0.15, false, false);

    fox_standing = engine.graphics.get_animated_sprite("fox_standing", 0.15, false, false);
    fox_running = engine.graphics.get_animated_sprite("fox_running", 0.11, false, false);
    fox_jumping = engine.graphics.get_animated_sprite("fox_jumping", 0.32, false, false);
    fox_attacking = engine.graphics.get_animated_sprite("fox_attacking", 0.07, false, false);
    fox_hurting = engine.graphics.get_animated_sprite("fox_hurting", 0.02, false, false);

    sprite = standing.get_sprite();

        ninja_star.sprite = engine.graphics.get_sprite("ninja_star");
    ninja_star.combat.invincible = true;
    ninja_star.combat.attack_damage = 2;
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
