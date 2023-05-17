#pragma once

#include "animatedsprite.h"
#include "command.h"
#include "graphics.h"
#include <functional>

class Enemy;
class Engine;

class EnemyType
{
public:
    AnimatedSprite animation;
    Vec<double> acceleration;
    int health, damage, hurting_duration;
    double elapsed_time;
    std::function<std::unique_ptr<Command>(Engine &engine, Enemy &enemy)> behavior;
};

EnemyType create_enemy_type(Graphics &raphics, std::string type_name);

std::unique_ptr<Command> default_behavior(Engine &, Enemy &enemy);
std::unique_ptr<Command> standing_behavior(Engine &, Enemy &enemy);
std::unique_ptr<Command> hurting(Engine &, Enemy &enemy);

EnemyType create_troll(Graphics &graphics);

EnemyType create_monster(Graphics &graphics);

EnemyType create_rat(Graphics &graphis);
EnemyType create_rat_running(Graphics &graphics);

EnemyType create_archer(Graphics &graphis);
EnemyType create_archer_running(Graphics &graphics);

EnemyType create_boss(Graphics &graphis);
EnemyType create_boss_running(Graphics &graphics);
