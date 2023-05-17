#pragma once
#include <memory>
#include <vector>
#include <string>
#include "projectile.h"

// forward declare

class Object;
class World;
class Engine;

class Command
{
public:
    virtual ~Command() {}
    virtual void execute(Object &object, Engine &engine) = 0;
};

class Stop : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class EnemyStop : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class Run : public Command
{
public:
    Run(double acceleration);
    void execute(Object &object, Engine &engine) override;

private:
    double acceleration;
};

class EnemyRun : public Command
{
public:
    EnemyRun(double acceleration);
    void execute(Object &object, Engine &engine) override;

private:
    double acceleration;
};

class Jump : public Command
{
public:
    Jump(double velocity);
    void execute(Object &object, Engine &engine) override;

private:
    double velocity;
};

class Attack : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class Wall : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class FireProjectile : public Command
{
public:
    FireProjectile(Projectile projectile, Vec<double> position, Vec<double> velocity);
    void execute(Object &object, Engine &engine) override;

private:
    Projectile projectile;
};

class DamageTile : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class HealthTile : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class EndGame : public Command
{
public:
    void execute(Object &object, Engine &engine) override;
};

class PlaySound : public Command
{
public:
    PlaySound(std::string sound_name, bool is_background);
    void execute(Object &object, Engine &engine) override;

private:
    std::string sound_name;
    bool is_background;
};

class LoadLevel : public Command
{
public:
    LoadLevel(const std::string &filename);
    void execute(Object &object, Engine &engine) override;

private:
    std::string filename;
};

std::shared_ptr<Command> create_command(std::string command_name, std::vector<std::string> arguments);
