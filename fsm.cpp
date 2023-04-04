#include "fsm.h"

#include "command.h"
#include "engine.h"
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
        // if (event.type == SDL_KEYUP) {
        //     SDL_Keycode key = event.key.keysym.sym;
        //     if (key == SDLK_RIGHT || key == SDLK_LEFT) {
        //         return std::make_unique<Standing>();
        //     }
        // }
    }
    return nullptr;
}
std::unique_ptr<State> Standing::update(Player &player, Engine &engine,
                                        double dt)
{
    State::update(player, engine, dt);
    player.physics.velocity.x *= damping;

    player.standing.update(dt);
    player.sprite = player.standing.get_sprite();
    // if (on_platform) {
    //     player.physics.velocity.y *= damping;
    // }
    // Vec<double> left_foot{player.physics.position.x,
    //                       player.physics.position.y - 1e-2};
    // Vec<double> right_foot{player.physics.position.x + player.size.x,
    //                        player.physics.position.y - 1e-2};
    // if (world.collides(left_foot) && !(world.collides(right_foot))) {
    //     player.physics.velocity.y = 0;
    // }

    if (on_left_wall(player, engine) || on_right_wall(player, engine))
    {
        return std::make_unique<OnWall>();
    }

    // slide off a platform, return new state?
    return nullptr;
}

void Standing::enter(Player &player)
{
    player.next_command = std::make_unique<Stop>();
    player.standing.reset();
    player.standing.flip(player.sprite.flip);
}

/////////
// Running
////////

std::unique_ptr<State> Running::handle_input(Player &, const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        // if (key == SDLK_RIGHT) {
        //     player.physics.acceleration.x = player.walk_acceleration;
        // } else if (key == SDLK_LEFT) {
        //     player.physics.acceleration.x = -player.walk_acceleration;
        // }
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
    player.running.update(dt);
    player.sprite = player.running.get_sprite();

    if (player.physics.velocity.x == 0)
    {
        return std::make_unique<Standing>();
    }
    // if (!on_platform(player, world)) {  // run off platform
    //     return std::make_unique<InAir>();
    // }

    if (on_left_wall(player, engine) || on_right_wall(player, engine))
    {
        return std::make_unique<OnWall>();
    }
    // Vec<double> left_foot{player.physics.position.x,
    //                       player.physics.position.y - 1e-2};
    // Vec<double> right_foot{player.physics.position.x + player.size.x,
    //                        player.physics.position.y - 1e-2};
    // if (world.collides(left_foot) && !(world.collides(right_foot))) {
    //     return std::make_unique<OnWall>();
    // }
    return nullptr;
}
void Running::enter(Player &player)
{
    player.next_command = std::make_unique<Run>(acceleration);
    player.running.reset();
    player.running.flip(acceleration < 0);
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
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_RIGHT)
        {
            return std::make_unique<Running>(player.walk_acceleration / 4);
        }
        else if (key == SDLK_LEFT)
        {
            return std::make_unique<Running>(-player.walk_acceleration / 4);
        }
    }
    return nullptr;
}

std::unique_ptr<State> Jumping::update(Player &player, Engine &engine,
                                       double dt)
{
    State::update(player, engine, dt);
    player.jumping.update(dt);
    player.sprite = player.jumping.get_sprite();

    if (player.physics.velocity.y == 0)
    {
        return std::make_unique<Standing>();
    }
    // Vec<double> left_foot{player.physics.position.x,
    //                       player.physics.position.y - 1e-2};
    // if (world.collides(left_foot)) {
    //     return std::make_unique<Standing>();
    // }

    // Vec<double> left_foot{player.physics.position.x,
    //                       player.physics.position.y - 1e-2};
    // Vec<double> right_foot{player.physics.position.x + player.size.x,
    //                        player.physics.position.y - 1e-2};
    // if (world.collides(left_foot) && !(world.collides(right_foot))) {
    //     return std::make_unique<OnWall>();
    // }

    if (on_left_wall(player, engine) || on_right_wall(player, engine))
    {
        return std::make_unique<OnWall>();
    }

    // if(!collides){InAir}
    return nullptr;
}

void Jumping::enter(Player &player)
{
    player.next_command = std::make_unique<Jump>(player.jump_velocity);
    player.jumping.flip(player.sprite.flip);
}

/////////
// OnWall
////////

std::unique_ptr<State> OnWall::handle_input(Player &player,
                                            const SDL_Event &event)
{
    // reduced movement in left and right direcitons
    // space -> add a small vy
    // down -> drop faster
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP)
        {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_RIGHT)
        {
            return std::make_unique<Running>(player.walk_acceleration / 4);
        }
        else if (key == SDLK_LEFT)
        {
            return std::make_unique<Running>(-player.walk_acceleration / 4);
        }
    }
    return nullptr;
}

std::unique_ptr<State> OnWall::update(Player &player, Engine &engine,
                                      double dt)
{
    State::update(player, engine, dt);
    player.physics.velocity.y = 0;

    // if (player.physics.velocity.y == 0)
    // {
    //     return std::make_unique<Standing>();
    // }
    // Vec<double> left_foot{player.physics.position.x,
    //                       player.physics.position.y - 1e-2};
    // Vec<double> right_foot{player.physics.position.x + player.size.x,
    //                        player.physics.position.y - 1e-2};
    // if (world.collides(left_foot) && !(world.collides(right_foot))) {
    //     return std::make_unique<OnWall>();
    // }
    // if(!collides){InAir}
    return nullptr;
}

void OnWall::enter(Player &player)
{
    player.next_command = std::make_unique<Wall>();
}

////////
// InAir
////////

// std::unique_ptr<State> InAir::handle_input(Player& player,
//                                            const SDL_Event& event) {
//     if (event.type == SDL_KEYDOWN) {
//         SDL_Keycode key = event.key.keysym.sym;
//         if (key == SDLK_SPACE || key == SDLK_UP) {
//             return std::make_unique<Jumping>();
//         } else if (key == SDLK_RIGHT) {
//             return std::make_unique<Running>(player.walk_acceleration / 4);
//         } else if (key == SDLK_LEFT) {
//             return std::make_unique<Running>(-player.walk_acceleration / 4);
//         }
//     }
//     return nullptr;
// }

// std::unique_ptr<State> InAir::update(Player& player, Engine& engine, double
// dt)
// {
//     State::update(player, world, dt);
//     return nullptr;
// }

// void InAir::enter(Player& player) {
//     player.next_command = std::make_unique<Air>();
// }

// void exit(Player& player) { player.physics.acceleration.y = 0; }
