#pragma once

#include <memory>

#include "audio.h"
#include "camera.h"
#include "graphics.h"
#include "world.h"

// forward declare
class Player;
class Settings;

class Engine
{
public:
    Engine(const Settings &settings);
    void load_level(const std::string &level_filename);
    void run();

    Graphics graphics;
    Camera camera;
    std::shared_ptr<World> world;
    Audio audio;

    std::shared_ptr<Player> player;

private:
    bool grid_on{false};
    bool running{true};
    void input();
    void update(double dt);
    void render();
};
