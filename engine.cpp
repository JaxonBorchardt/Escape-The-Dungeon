#include "engine.h"

#include <chrono>

#include "player.h"
#include "settings.h"
#include "level.h"

Engine::Engine(const Settings &settings)
    : graphics{settings.title, settings.screen_width, settings.screen_height},
      camera{graphics, settings.titlesize}
// world{31, 20}
{
    load_level(settings.starting_level);
}

void Engine::load_level(const std::string &level_filename)
{
    // world.add_platform(0, 0, 30, 1);  // floor
    // world.add_platform(0, 0, 1, 10);  // left wall
    // world.add_platform(30, 0, 1, 10); // right wall
    // // world.add_platform(0, 10, 30, 1); ceiling

    // world.add_platform(4, 6, 4, 1);
    // world.add_platform(12, 4, 6, 1);
    // world.add_platform(25, 2, 3, 1);

    Level level{level_filename, graphics, audio};
    audio.play_sound("background", true);
    world = std::make_shared<World>(level);

    // add player
    player =
        std::make_shared<Player>(*this, Vec<double>{10, 4}, Vec<int>{1, 1});

    // move camera
    camera.move_to(player->physics.position);
}

void Engine::input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // handle windows and systems events first
        if (event.type == SDL_QUIT)
        {
            running = false;
            break;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g)
        {
            grid_on = !grid_on;
        }
        auto command = player->handle_input(event);
        if (command)
        {
            command->execute(*player, *this);
        }

        // pass the rest of the events to the player who will
        // react to keypresses by moving
        player->handle_input(event);
    }
}

void Engine::update(double dt)
{
    player->update(*this, dt);
    // camera.update(dt);
}

void Engine::render()
{
    graphics.clear();
    camera.render(world->backgrounds);
    camera.render(world->tilemap, grid_on);
    auto [position, color] = player->get_sprite();
    camera.move_to(position);
    camera.render(position, color);
    camera.render(*player);
    graphics.update();
}

void Engine ::run()
{
    running = true;
    constexpr double dt = 1.0 / 60.0;
    auto previous = std::chrono::high_resolution_clock::now();
    double lag{0.0};
    while (running)
    {
        auto current =
            std::chrono::high_resolution_clock::now(); // current time
        std::chrono::duration<double> elapsed = current - previous;
        previous = current;
        // runtime += elapsed.count();
        lag += elapsed.count();

        input();
        while (lag >= dt)
        {
            update(dt);
            lag -= dt;
        }
        render();
    }
}