#pragma once
#include "vec.h"

// forward declare
class Graphics;
class Tilemap;
class Color;
class Sprite;
class Object;
class Physics;

class Camera
{
public:
    Camera(Graphics &graphics, int tilesize);

    void move_to(const Vec<double> &new_locaiton);
    Vec<int> world_to_screen(const Vec<double> &world_position) const;

    void render(const Vec<double> &position, const Color &color,
                bool filled = true) const;
    void render(const Tilemap &tilemap, bool grid_on = false) const;
    void render(const Vec<double> &position, const Sprite &sprite) const;
    void render(const Object &object) const;
    void render(const std::vector<std::pair<Sprite, int>> &backgrounds) const;

    void render_life(int life, int max_life);

private:
    Graphics &graphics;
    // Physics &physics;
    int tilesize;
    Vec<double> location; // camera position in world coordinates

    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
};
