#include "fsm.h"

#include "command.h"
#include "engine.h"
#include "randomness.h"
#include "player.h"
#include "world.h"

Running::Running(double acceleration) : acceleration{acceleration} {}

///////////
// State
//////////
std::unique_ptr<State> State::update(Player &player, Engine &engine,
                                     double dt)
{
    Physics old = player.physics;
    player.physics.update(dt);

    // attempt to move in x first
    Vec<double> future{player.physics.position.x, old.position.y};
    Vec<double> vx{player.physics.velocity.x, 0};
    engine.world->move_to(future, player.size, vx);
    // attempt to move in y
    Vec<double> vy{0, player.physics.velocity.y};
    future.y = player.physics.position.y;
    engine.world->move_to(future, player.size, vy);

    // update position and velocity of player
    player.physics.position = future;
    player.physics.velocity = {vx.x, vy.y};

    // see if we touch a tile with a command
    auto command = engine.world->touch_tiles(player);
    if (command)
    {
        command->execute(player, engine);
    }

    return nullptr;
}
////////
// Standing
////////

bool on_platform(const Player &player, const Engine &engine)
{
    constexpr double epsilon = 1e-2;
    Vec<double> left_foot{player.physics.position.x,
                          player.physics.position.y - epsilon};
    Vec<double> right_foot{player.physics.position.x + player.size.x,
                           player.physics.position.y - epsilon};
    return engine.world->collides(left_foot) ||
           engine.world->collides(right_foot);
}

bool on_left_wall(const Player &player, const Engine &engine)
{
    constexpr double epsilon = 1e-2;
    Vec<double> left_foot{player.physics.position.x - epsilon,
                          player.physics.position.y};
    Vec<double> left_hand{player.physics.position.x - epsilon,
                          player.physics.position.y + player.size.y};
    return engine.world->collides(left_hand) && engine.world->collides(left_foot);
}

bool on_right_wall(const Player &player, const Engine &engine)
{
    constexpr double epsilon = 1e-2;
    Vec<double> right_foot{player.physics.position.x + player.size.x + epsilon,
                           player.physics.position.y};
    Vec<double> right_hand{player.physics.position.x + player.size.x + epsilon,
                           player.physics.position.y + player.size.y};

    return engine.world->collides(right_hand) &&
           engine.world->collides(right_foot);
}

std::unique_ptr<State> Standing::handle_input(Player &player,
                                              const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_RIGHT)
        {
            return std::make_unique<Running>(player.walk_acceleration);
        }
        else if (key == SDLK_LEFT)
        {
            return std::make_unique<Running>(-player.walk_acceleration);
        }
        else if (key == SDLK_s)
        {
            return std::make_unique<Attacking>();
        }
        else if (key == SDLK_q)
        {
            return std::make_unique<AttackAll>();
        }
        else if (key == SDLK_f)
        {
            Vec<double> position{player.physics.position.x + player.size.x / 1.5, player.physics.position.y + player.size.y / 1.5};
            Vec<double> velocity{1200, 1};

            if (player.sprite.flip)
            {
                position = {player.physics.position.x, player.physics.position.y + player.size.y / 1.5};
                velocity.x *= -1;
            }
            player.next_command = std::make_unique<FireProjectile>(player.ninja_star, position, velocity);
        }
    }
    return nullptr;
}
std::unique_ptr<State> Standing::update(Player &player, Engine &engine,
                                        double dt)
{
    State::update(player, engine, dt);
    player.physics.velocity.x *= damping;

    player.fox_standing.update(dt);
    player.sprite = player.fox_standing.get_sprite();
    return nullptr;
}

void Standing::enter(Player &player)
{
    player.next_command = std::make_unique<Stop>();
    player.fox_standing.reset();
    player.fox_standing.flip(player.sprite.flip);
}

/////////
// Running
////////

std::unique_ptr<State> Running::handle_input(Player &player, const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_s)
        {
            return std::make_unique<Attacking>();
        }
        else if (key == SDLK_f)
        {
            Vec<double> position{player.physics.position.x + player.size.x / 1.5, player.physics.position.y + player.size.y / 1.5};
            Vec<double> velocity{1200, 1};

            if (player.sprite.flip)
            {
                position = {player.physics.position.x, player.physics.position.y + player.size.y / 1.5};
                velocity.x *= -1;
            }
            player.next_command = std::make_unique<FireProjectile>(player.ninja_star, position, velocity);
        }
    }
    if (event.type == SDL_KEYUP)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if ((key == SDLK_RIGHT || key == SDLK_LEFT))
        {
            return std::make_unique<Standing>();
        }
    }
    return nullptr;
}
std::unique_ptr<State> Running::update(Player &player, Engine &engine,
                                       double dt)
{
    State::update(player, engine, dt);
    player.fox_running.update(dt);
    player.sprite = player.fox_running.get_sprite();

    if (player.physics.velocity.x == 0)
    {
        return std::make_unique<Standing>();
    }
    return nullptr;
}
void Running::enter(Player &player)
{
    player.next_command = std::make_unique<Run>(acceleration);
    player.fox_running.reset();
    player.fox_running.flip(acceleration < 0);
}
void Running::exit(Player &player) { player.physics.acceleration.x = 0; }

/////////
// Jumping
////////
std::unique_ptr<State> Jumping::handle_input(Player &player,
                                             const SDL_Event &event)
{
    // reduced movement in left and right direcitons
    // space -> add a small vy
    // down -> drop faster
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_RIGHT)
        {
            return std::make_unique<Running>(player.walk_acceleration / 2);
        }
        else if (key == SDLK_LEFT)
        {
            return std::make_unique<Running>(-player.walk_acceleration / 2);
        }
        else if (key == SDLK_s)
        {
            return std::make_unique<Attacking>();
        }
        else if (key == SDLK_f)
        {
            Vec<double> position{player.physics.position.x + player.size.x / 1.5, player.physics.position.y + player.size.y / 1.5};
            Vec<double> velocity{1200, 1};

            if (player.sprite.flip)
            {
                position = {player.physics.position.x, player.physics.position.y + player.size.y / 1.5};
                velocity.x *= -1;
            }
            player.next_command = std::make_unique<FireProjectile>(player.ninja_star, position, velocity);
        }
    }
    return nullptr;
}

std::unique_ptr<State> Jumping::update(Player &player, Engine &engine,
                                       double dt)
{
    State::update(player, engine, dt);
    player.fox_jumping.update(dt);
    player.sprite = player.fox_jumping.get_sprite();

    if (player.physics.velocity.y == 0)
    {
        return std::make_unique<Standing>();
    }
    return nullptr;
}

void Jumping::enter(Player &player)
{
    player.next_command = std::make_unique<Jump>(player.jump_velocity);
    player.fox_jumping.reset();
    player.fox_jumping.flip(player.sprite.flip);
}

/////////
// Attack
////////

std::unique_ptr<State> Attacking::handle_input(Player &player,
                                               const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_RIGHT)
        {
            return std::make_unique<Running>(player.walk_acceleration / 2);
        }
        else if (key == SDLK_LEFT)
        {
            return std::make_unique<Running>(-player.walk_acceleration / 2);
        }
    }
    return nullptr;
}

std::unique_ptr<State> Attacking::update(Player &player, Engine &engine,
                                         double dt)
{
    State::update(player, engine, dt);
    player.fox_attacking.update(dt);
    player.sprite = player.fox_attacking.get_sprite();

    for (auto enemy : engine.world->enemies)
    {
        player.combat.attack(*enemy);
    }

    return nullptr;
}

void Attacking::enter(Player &player)
{
    player.next_command = std::make_unique<Attack>();
    player.fox_attacking.reset();
    player.fox_attacking.flip(player.sprite.flip);
}

/////////
// AttackAll
//////////

std::unique_ptr<State> AttackAll::handle_input(Player &,
                                               const SDL_Event &event)
{
    if (event.type == SDL_KEYUP)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_q)
        {
            return std::make_unique<Standing>();
        }
    }
    return nullptr;
}

std::unique_ptr<State> AttackAll::update(Player &player, Engine &engine,
                                         double)
{
    for (auto enemy : engine.world->enemies)
    {
        player.combat.attack(*enemy);
    }
    return std::make_unique<Standing>();
}

void AttackAll::enter(Player &) {}

////////////
// Hurting
///////////

std::unique_ptr<State> Hurting::handle_input(Player &player,
                                             const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT)
        {
            player.next_command = std::make_unique<Run>(-player.walk_acceleration / 2);
            player.sprite.flip = true;
        }
        else if (key == SDLK_RIGHT)
        {
            player.next_command = std::make_unique<Run>(player.walk_acceleration / 2);
            player.sprite.flip = false;
        }
    }
    if (event.type == SDL_KEYUP)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT)
        {
            player.next_command = std::make_unique<Run>(0);
        }
    }
    return nullptr;
}

std::unique_ptr<State> Hurting::update(Player &player, Engine &engine,
                                       double dt)
{
    elapsed_time += dt;
    if (elapsed_time >= cooldown)
    {
        return std::make_unique<Standing>();
    }

    State::update(player, engine, dt);
    player.fox_hurting.update(dt);
    player.sprite = player.fox_hurting.get_sprite();
    player.physics.apply_friction(damping);

    return nullptr;
}

void Hurting::enter(Player &player)
{
    elapsed_time = 0;
    player.combat.invincible = true;
}

void Hurting::exit(Player &player)
{
    player.combat.invincible = false;
}

