#pragma once

// forward declare

class Player;
class World;
class Engine;

class Command {
   public:
    virtual ~Command() {}
    virtual void execute(Player& player, Engine& engine) = 0;
};

class Stop : public Command {
   public:
    void execute(Player& player, Engine& engine) override;
};

class Run : public Command {
   public:
    Run(double acceleration);
    void execute(Player& player, Engine& engine) override;

   private:
    double acceleration;
};

class Jump : public Command {
   public:
    Jump(double velocity);
    void execute(Player& player, Engine& engine) override;

   private:
    double velocity;
};

class Wall : public Command {
   public:
    void execute(Player& player, Engine& engine) override;
};

// class Air : public Command {
//    public:
//     Air(double velocity);
//     void execute(Player& player, Engine& engine) override;

//    private:
//     double velocity;
// };