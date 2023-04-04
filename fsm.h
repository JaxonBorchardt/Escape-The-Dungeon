#pragma once

#include <SDL2/SDL.h>

#include <memory>

// forward declaration
class Player;
class World;
class Engine;

class State {
   public:
    virtual ~State() {}
    virtual std::unique_ptr<State> handle_input(Player& player,
                                                const SDL_Event& event) = 0;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine,
                                          double dt);
    virtual void enter(Player&){};
    virtual void exit(Player&){};
};

class Standing : public State {
    virtual std::unique_ptr<State> handle_input(
        Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine,
                                          double dt);
    void enter(Player& player) override;
};

class Running : public State {
   public:
    Running(double acceleration);
    virtual std::unique_ptr<State> handle_input(
        Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine,
                                          double dt);
    void enter(Player& player) override;
    void exit(Player& player);

   private:
    double acceleration;
};

class Jumping : public State {
    virtual std::unique_ptr<State> handle_input(
        Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine,
                                          double dt);
    void enter(Player& player) override;
};

class OnWall : public State {
    virtual std::unique_ptr<State> handle_input(
        Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine,
                                          double dt);
    void enter(Player& player) override;
};

// class InAir : public State {
//     virtual std::unique_ptr<State> handle_input(
//         Player& player, const SDL_Event& event) override;
//     virtual std::unique_ptr<State> update(Player& player, Engine& engine,
//                                           double dt);
//     void enter(Player& player) override;
//     // void exit(Player& player);
// };