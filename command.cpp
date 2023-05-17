#include "command.h"

#include "engine.h"
#include "object.h"
#include "world.h"

////////
// Stop
///////

void Stop::execute(Object &object, Engine &engine)
{

    object.physics.acceleration.x = 0;
    engine.audio.play_sound("standing");
}

void EnemyStop::execute(Object &object, Engine &)
{

    object.physics.acceleration.x = 0;
}

////////
// Running
///////

Run::Run(double acceleration) : acceleration{acceleration} {}

void Run::execute(Object &object, Engine &engine)
{

    object.physics.acceleration.x = acceleration;
    engine.audio.play_sound("running");
}

EnemyRun::EnemyRun(double acceleration) : acceleration{acceleration} {}

void EnemyRun::execute(Object &object, Engine &)
{

    object.physics.acceleration.x = acceleration;
}

////////
// Jump
///////

Jump::Jump(double velocity) : velocity{velocity} {}

void Jump::execute(Object &object, Engine &engine)
{

    object.physics.velocity.y = velocity;
    engine.audio.play_sound("jumping");
}

/////////
// Attack
////////

void Attack::execute(Object &, Engine &engine)
{
    engine.audio.play_sound("attacking");
}

////////////
// FireProjectile
///////////////

FireProjectile::FireProjectile(Projectile projectile, Vec<double> position, Vec<double> velocity)
    : projectile{projectile}
{
    this->projectile.physics.position = position;
    this->projectile.physics.velocity = velocity * 1.2;
    this->projectile.physics.acceleration.y = gravity;
}

void FireProjectile::execute(Object &, Engine &engine)
{
    engine.world->projectiles.push_back(projectile);
    engine.audio.play_sound("ninja-star");
}

///////
// Wall
//////
void Wall::execute(Object &object, Engine &)
{

    object.physics.velocity.y = 0;
}

///////////
// Game Changes
//////////

void DamageTile::execute(Object &object, Engine &)
{
    object.combat.take_damage(2);
}

void HealthTile::execute(Object &object, Engine &)
{
    object.combat.take_damage(-2);
}

void EndGame::execute(Object &, Engine &engine)
{
    engine.stop();
}

PlaySound::PlaySound(std::string sound_name, bool is_background)
    : sound_name{sound_name}, is_background{is_background} {}

void PlaySound::execute(Object &, Engine &engine)
{
    engine.audio.play_sound(sound_name, is_background);
}
LoadLevel::LoadLevel(const std::string &filename)
    : filename{filename} {}

void LoadLevel::execute(Object &, Engine &engine)
{
    engine.next_level = "assets/" + filename;
}
std::shared_ptr<Command> create_command(std::string command_name, std::vector<std::string> arguments)
{
    if (command_name == "end_game")
    {
        return std::make_shared<EndGame>();
    }
    else if (command_name == "damage_tile")
    {

        return std::make_shared<DamageTile>();
    }
    else if (command_name == "health_tile")
    {
        return std::make_shared<HealthTile>();
    }
    else if (command_name == "play_sound")
    {
        if (arguments.size() != 2)
        {
            throw std::runtime_error("play sound requires 2 arguments ");
        }
        bool is_background = arguments.at(1) == "true" ? true : false;
        std::string sound_name = arguments.at(0);
        return std::make_shared<PlaySound>(sound_name, is_background);
    }
    else if (command_name == "load_level")
    {
        if (arguments.size() != 1)
        {
            throw std::runtime_error("load_level must be followed by a level filename");
        }
        return std::make_shared<LoadLevel>(arguments.front());
    }
    throw std::runtime_error("unknown command: " + command_name);
}
