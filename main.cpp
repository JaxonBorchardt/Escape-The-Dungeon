
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>
#include <queue>

#include "camera.h"
#include "command.h"
#include "graphics.h"
#include "player.h"
#include "vec.h"
#include "world.h"

int main() {
    Graphics graphics{"game", 1280, 720};

    // objects
    Player player{{10, 4}, {1, 1}};

    World world{31, 20};
    // boundary walls
    world.add_platform(0, 0, 30, 1);   // floor
    world.add_platform(0, 0, 1, 10);   // left wall
    world.add_platform(30, 0, 1, 10);  // right wall
    // world.add_platform(0, 10, 30, 1); ceiling

    // platforms
    world.add_platform(4, 6, 4, 1);
    world.add_platform(12, 4, 6, 1);
    world.add_platform(25, 2, 3, 1);

    // camera
    int tilesize{64};
    Camera camera{graphics, tilesize};
    camera.move_to({10, 5});

    // std::queue<std::pair<double, std::unique_ptr<Command>>> script;
    // script.push({3, std::make_unique<Run>(40)});
    // script.push({5, std::make_unique<Jump>(25)});
    // script.push({7, std::make_unique<Stop>()});

    bool grid_on{false};
    bool running{true};
    auto previous = std::chrono::high_resolution_clock::now();  // previous time
    // double runtime{0.0};
    double lag{0.0};
    while (running) {
        auto current =
            std::chrono::high_resolution_clock::now();  // current time
        std::chrono::duration<double> elapsed = current - previous;
        previous = current;
        // runtime += elapsed.count();
        lag += elapsed.count();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // handle windows and systems events first
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
                grid_on = !grid_on;
            }
            auto command = player.handle_input(event);
            if (command) {
                command->execute(player, world);
            }

            // if (!script.empty()) {
            //     double time = script.front().first;
            //     if (runtime >= time) {
            //         auto command = std::move(script.front().second);

            //         script.pop();
            //         command->execute(player, world);
            //     }
            // }
            // pass the rest of the events to the player who will
            // react to keypresses by moving
            player.handle_input(event);
        }

        // move the player in the world
        constexpr double dt = 1.0 / 60.0;
        while (lag >= dt) {
            player.update(world, dt);
            lag -= dt;
        }

        // camera.update(dt)

        // draw the player and platforms
        graphics.clear();
        camera.render(world.tilemap, grid_on);
        auto [position, color] = player.get_sprite();
        camera.move_to(position);
        camera.render(position, color);
        graphics.update();
    }
}
