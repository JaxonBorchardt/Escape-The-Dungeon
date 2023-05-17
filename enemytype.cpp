#include "enemytype.h"
#include "enemy.h"

EnemyType create_enemy_type(Graphics &graphics, std::string type_name)
{
    if (type_name == "troll")
    {
        return create_troll(graphics);
    }
    else if (type_name == "monster")
    {
        return create_monster(graphics);
    }
    else if (type_name == "rat")
    {
        return create_rat(graphics);
    }
    else if (type_name == "rat_running")
    {
        return create_rat_running(graphics);
    }
    else if (type_name == "archer")
    {
        return create_archer(graphics);
    }
    else if (type_name == "archer_running")
    {
        return create_archer_running(graphics);
    }
    else if (type_name == "boss")
    {
        return create_boss(graphics);
    }
    else if (type_name == "boss_running")
    {
        return create_boss_running(graphics);
    }
    else
    {
        throw std::runtime_error("Unknown enemy type: " + type_name);
    }
}

std::unique_ptr<Command> default_behavior(Engine &, Enemy &enemy)
{
    if (abs(enemy.last_edge_position.x - enemy.physics.position.x) > 5)
    {
        enemy.last_edge_position.x = enemy.physics.position.x;
        enemy.physics.acceleration.x = -enemy.physics.acceleration.x;
    }
    return std::make_unique<EnemyRun>(enemy.physics.acceleration.x);
}

std::unique_ptr<Command> standing_behavior(Engine &, Enemy &)
{
    return std::make_unique<EnemyStop>();
}

std::unique_ptr<Command> hurting(Engine &, Enemy &)
{
    return std::make_unique<EnemyStop>();
}

EnemyType create_troll(Graphics &graphics)
{
    Vec<double> acceleration{-15, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("troll_running", 0.15, true, false);
    return EnemyType{sprite, acceleration, 8, 2, 2, 0, default_behavior};
}

EnemyType create_monster(Graphics &graphics)
{
    Vec<double> acceleration{0, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("monster_standing", 0.15, true, false);
    return EnemyType{sprite, acceleration, 5, 3, 3, 0, standing_behavior};
}

EnemyType create_rat(Graphics &graphics)
{
    Vec<double> acceleration{-5, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("rat_standing", 0.15, true, false);
    return EnemyType{sprite, acceleration, 5, 2, 2, 0, standing_behavior};
}

EnemyType create_rat_running(Graphics &graphics)
{
    Vec<double> acceleration{-5, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("rat_running", 0.15, true, false);
    return EnemyType{sprite, acceleration, 5, 2, 2, 0, default_behavior};
}

EnemyType create_archer(Graphics &graphics)
{
    Vec<double> acceleration{-5, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("archer_standing", 0.15, true, false);
    return EnemyType{sprite, acceleration, 5, 2, 2, 0, standing_behavior};
}

EnemyType create_archer_running(Graphics &graphics)
{
    Vec<double> acceleration{-5, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("archer_running", 0.15, true, false);
    return EnemyType{sprite, acceleration, 5, 2, 2, 0, default_behavior};
}

EnemyType create_boss(Graphics &graphics)
{
    Vec<double> acceleration{-10, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("boss_standing", 0.15, true, false);
    return EnemyType{sprite, acceleration, 8, 2, 1, 0, standing_behavior};
}
EnemyType create_boss_running(Graphics &graphics)
{
    Vec<double> acceleration{-10, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("boss_running", 0.15, true, false);
    return EnemyType{sprite, acceleration, 8, 2, 1, 0, default_behavior};
}
