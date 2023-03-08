#pragma once

// forward declare

class Player;
class World;

class Command {
   public:
    virtual ~Command() {}
    virtual void execute(Player& player, World& world) = 0;
};

class Stop : public Command {
   public:
    void execute(Player& player, World& world) override;
};

class Run : public Command {
   public:
    Run(double acceleration);
    void execute(Player& player, World& world) override;

   private:
    double acceleration;
};

class Jump : public Command {
   public:
    Jump(double velocity);
    void execute(Player& player, World& world) override;

   private:
    double velocity;
};