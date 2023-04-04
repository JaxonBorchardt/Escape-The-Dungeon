#pragma once

#include <string>
#include <unordered_map>
#include "vec.h"
#include "sprite.h"
#include "tilemap.h"

class Graphics;
class Audio;

class Level
{
public:
    Level(const std::string &filename, Graphics &graphics, Audio &audio);

    std::string filename;
    int width, height; // tilemap
    Vec<double> player_start_location{-1, -1};

    std::unordered_map<char, Tile> tile_types;
    std::vector<std::pair<Vec<int>, Tile>> tiles;    // tiles need to change for each level
    std::vector<std::pair<Sprite, int>> backgrounds; // distance from player, move slower

private:
    void load(Graphics &graphics, Audio &audio);
    void load_theme(const std::string &theme_filename, Graphics &graphics, Audio &audio);
};