#include "level.h"
#include <fstream>
#include <algorithm>
#include "graphics.h"
#include "audio.h"

Level::Level(const std::string &filename, Graphics &graphics, Audio &audio)
    : filename{filename}
{
    load(graphics, audio);
}

void Level::load(Graphics &graphics, Audio &audio)
{
    std::ifstream input{filename};
    if (!input)
    {
        throw std::runtime_error("Level::level Could not open file: " + filename);
    }
    // error if cant open file

    // load the level's theme
    std::string theme_filename;
    input >> theme_filename;
    load_theme(theme_filename, graphics, audio);

    // read the level's layout
    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line);)
    {
        if (line.empty())
        {
            continue;
        }
        lines.push_back(line);
    }
    // set level dimensions
    height = lines.size();
    width = lines.front().size();

    // ensure rectangular
    bool rectangular = std::all_of(std::begin(lines), std::end(lines), [=](const std::string &line)
                                   {
                                       return static_cast<int>(line.size()) == width;
                                       // error handle!
                                   });

    for (int y = 0; y < height; ++y)
    { // y is the row of the image
        for (int x = 0; x < width; ++x)
        {
            char symbol = lines.at(y).at(x);
            if (std::isspace(symbol))
            {
                continue;
            }
            if (symbol == 'P')
            { // player starting position
                player_start_location = Vec<double>{static_cast<double>(x), static_cast<double>(height - y - 1)};
                continue;
            }
            // determine the tile type
            auto it = tile_types.find(symbol);
            if (it != tile_types.end())
            {
                Vec<int> position{x, height - y - 1};
                const Tile &tile = it->second;
                tiles.push_back({position, tile});
            }
            else
            { // error
            }
        }
    }
    // error handling for player starting locaiton == -1,-1
}

void Level::load_theme(const std::string &theme_filename, Graphics &graphics, Audio &audio)
{
    std::ifstream input{"assets/" + theme_filename};
    // error if cant open
    if (!input)
    {
        throw std::runtime_error("level::load_theme Could not open file: " + filename);
    }

    std::string command;
    for (int line = 0; input >> command; ++line)
    {
        if (command == "load-spritesheet")
        {
            std::string filename;
            input >> filename; // if you cant, error
            graphics.load_spritesheet("assets/" + filename);
            if (!input)
            {
                throw std::runtime_error("load-spritesheet Could not open file: " + filename);
            }
        }
        else if (command == "load-sounds")
        {
            std::string filename;
            input >> filename; // if you cant, error
            audio.load_sounds("assets/" + filename);
            if (!input)
            {
                throw std::runtime_error("load-sounds Could not open file: " + filename);
            }
        }
        else if (command == "backgorund")
        {
            std::string filename;
            int distance;
            input >> filename >> distance; // error handle
            Sprite background = graphics.load_image("assets/" + filename);
            if (!input)
            {
                throw std::runtime_error("background Could not open file: " + filename);
            }
            background.scale = 5; // FIX ME!!
            backgrounds.push_back({background, distance});
        }
        else if (command == "tile")
        {
            char symbol;
            std::string sprite_name;
            bool blocking;
            input >> symbol >> sprite_name >> std::boolalpha >> blocking; // error handle
            Sprite sprite = graphics.get_sprite(sprite_name);
            tile_types[symbol] = Tile{sprite, blocking};
        }
        else
        {
            // error unknown command on line
            // filename:line "error message"
        }
    }
}
