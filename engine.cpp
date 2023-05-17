#include "engine.h"

#include <chrono>

#include "player.h"
#include "settings.h"
#include "level.h"
#include "projectile.h"
#include "loadscreen.h"

Engine::Engine(const Settings &settings)
    : graphics{settings.title, settings.screen_width, settings.screen_height},
      camera{graphics, settings.titlesize}

{
    load_level(settings.starting_level);
    load_level(settings.first_level);
}

void Engine::load_level(const std::string &level_filename)
{

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
    for (auto enemy : world->enemies)
    {
        auto command = enemy->next_action(*this);
        if (command)
        {
            command->execute(*enemy, *this);
        }
    }
}

void Engine::update(double dt)
{
    player->update(*this, dt);

    for (auto enemy : world->enemies)
    {
        auto command = enemy->update(*this, dt);
        if (command)
        {
            command->execute(*enemy, *this);
        }
    }

    for (auto &projectile : world->projectiles)
    {
        projectile.update(*this, dt);
    }
    // handle collisions between enemy and player
    world->build_quadtree();
    AABB player_box{player->physics.position, {1.0 * player->size.x, 1.0 * player->size.y}};
    std::vector<Object *> enemies = world->quadtree.query_range(player_box);

    if (enemies.size() > 0)
    {
        auto enemy = enemies.front();
        enemy->combat.attack(*player);
        // enter the hurting state
        player->state->exit(*player);
        player->state = std::make_unique<Hurting>();
        player->state->enter(*player);
    }

    // handle projectile collisions
    for (auto &projectile : world->projectiles)
    {
        AABB projectile_box{projectile.physics.position, {1.0 * projectile.size.x, 1.0 * projectile.size.y}};
        std::vector<Object *> enemies = world->quadtree.query_range(projectile_box);
        for (auto enemy : enemies)
        {
            projectile.combat.attack(*enemy);
        }
    }

    // end game if player is dead
    if (!player->combat.is_alive)
    {
        EndGame endgame;
        endgame.execute(*player, *this);
        return;
    }

    // check for enemy/projectile death
    world->remove_inactive();

    // camera.update(dt);
}

void Engine::render()
{
    graphics.clear();
    camera.render(world->backgrounds);
    camera.render(world->tilemap, grid_on);
    camera.render_life(player->combat.health, player->combat.max_health);
    auto [position, color] = player->get_sprite();
    camera.move_to(position);
    camera.render(*player);
    for (auto enemy : world->enemies)
    {
        camera.render(*enemy);
    }
    for (auto projectile : world->projectiles)
    {
        camera.render(projectile);
    }
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
        if (next_level)
        {
            load_level(next_level.value());
            next_level.reset();
        }
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

    setup_end_screen();

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            { // closing the window
                running = false;
                break;
            }
        }
        graphics.clear();
        camera.render(world->backgrounds);
        graphics.update();
    }
}

void Engine::stop()
{
    running = false;
}

void Engine::setup_end_screen()
{
    running = true;
    Loadscreen game_over{"assets/game-over.txt", graphics, audio};
    world->backgrounds = game_over.backgrounds;
}