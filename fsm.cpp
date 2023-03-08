#include "fsm.h"

#include "command.h"
#include "player.h"
#include "world.h"

Running::Running(double acceleration) : acceleration{acceleration} {}

///////////
// State
//////////
std::unique_ptr<State> State::update(Player& player, World& world, double dt) {
    Physics old = player.physics;
    player.physics.update(dt);

    // attempt to move in x first
    Vec<double> future{player.physics.position.x, old.position.y};
    Vec<double> vx{player.physics.velocity.x, 0};
    world.move_to(future, player.size, vx);
    // attempt to move in y
    Vec<double> vy{0, player.physics.velocity.y};
    future.y = player.physics.position.y;
    world.move_to(future, player.size, vy);

    // update position and velocity of player
    player.physics.position = future;
    player.physics.velocity = {vx.x, vy.y};

    return nullptr;
}
////////
// Standing
////////

bool on_platform(const Player& player, const World& world) {
    constexpr double epsilon = 1e-2;
    Vec<double> left_foot{player.physics.position.x,
                          player.physics.position.y - epsilon};
    Vec<double> right_foot{player.physics.position.x + player.size.x,
                           player.physics.position.y - epsilon};
    return world.collides(left_foot) || world.collides(right_foot);
}

std::unique_ptr<State> Standing::handle_input(Player& player,
                                              const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        } else if (key == SDLK_RIGHT) {
            return std::make_unique<Running>(player.walk_acceleration);
        } else if (key == SDLK_LEFT) {
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
std::unique_ptr<State> Standing::update(Player& player, World& world,
                                        double dt) {
    State::update(player, world, dt);
    player.physics.velocity.x *= damping;

    // slide off a platform, return new state?
    return nullptr;
}

void Standing::enter(Player& player) {
    player.next_command = std::make_unique<Stop>();
}

/////////
// Running
////////

std::unique_ptr<State> Running::handle_input(Player& player,
                                             const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        }
        if (key == SDLK_RIGHT) {
            player.physics.acceleration.x = player.walk_acceleration;
        } else if (key == SDLK_LEFT) {
            player.physics.acceleration.x = -player.walk_acceleration;
        }
    }
    if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_RIGHT || key == SDLK_LEFT) {
            return std::make_unique<Standing>();
        }
    }
    return nullptr;
}
std::unique_ptr<State> Running::update(Player& player, World& world,
                                       double dt) {
    State::update(player, world, dt);

    if (player.physics.velocity.x == 0) {
        return std::make_unique<Standing>();
    }
    return nullptr;
}
void Running::enter(Player& player) {
    player.next_command = std::make_unique<Run>(acceleration);
}
void Running::exit(Player& player) { player.physics.acceleration.x = 0; }

/////////
// Jumping
////////
std::unique_ptr<State> Jumping::handle_input(Player& player,
                                             const SDL_Event& event) {
    // reduced movement in left and right direcitons
    // space -> add a small vy
    // down -> drop faster
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        } else if (key == SDLK_RIGHT) {
            return std::make_unique<Running>(player.walk_acceleration);
        } else if (key == SDLK_LEFT) {
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
std::unique_ptr<State> Jumping::update(Player& player, World& world,
                                       double dt) {
    State::update(player, world, dt);

    if (player.physics.velocity.y == 0) {
        // Vec<double> left_foot{player.position.x, player.position.y -
        // 1e-2}; if (world.collides(left_foot))// standing
        return std::make_unique<Standing>();
    }
    // if(!collides){InAir}
    return nullptr;
}

void Jumping::enter(Player& player) {
    player.next_command = std::make_unique<Jump>(player.jump_velocity);
}

////////
// InAir
////////

std::unique_ptr<State> InAir::handle_input(Player& player,
                                           const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        } else if (key == SDLK_RIGHT) {
            return std::make_unique<Running>(player.walk_acceleration);
        } else if (key == SDLK_LEFT) {
            return std::make_unique<Running>(-player.walk_acceleration);
        }
    }
}

std::unique_ptr<State> InAir::update(Player& player, World& world, double dt) {}

void InAir::enter(Player& player) {}