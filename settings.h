#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

class Settings
{
public:
    Settings(const std::string &filename);

    std::string filename;
    std::string title;
    int screen_width, screen_height;
    int titlesize;
    std::string starting_level;
    std::string first_level;
    std::string load_sounds;
    // sprites

private:
    void load();
    std::unordered_map<std::string, std::string> parameters;

    template <typename T>
    void load(const std::string &key, T &value)
    {
        try
        {
            std::stringstream ss{parameters.at(key)};
            ss >> value;
        }
        catch (std::out_of_range &)
        {
            throw std::runtime_error("key '" + key + "' not found in " +
                                     filename);
        }
    }
};