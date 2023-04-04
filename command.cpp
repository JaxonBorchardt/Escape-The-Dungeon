#include "command.h"

#include "engine.h"
#include "player.h"
#include "world.h"

////////
// Stop
///////

void Stop::execute(Player &player, Engine &engine)
{
    player.color = {255, 0, 0, 255}; // green for stop
    // player.physics.velocity.y = 0;
    player.physics.acceleration.x = 0;
    engine.audio.play_sound("standing");
}

////////
// Running
///////

Run::Run(double acceleration) : acceleration{acceleration} {}

void Run::execute(Player &player, Engine &engine)
{
    player.color = {255, 255, 0, 255}; // yellow for running
    player.physics.acceleration.x = acceleration;
    engine.audio.play_sound("running");
}

////////
// Jump
///////

Jump::Jump(double velocity) : velocity{velocity} {}

void Jump::execute(Player &player, Engine &engine)
{
    player.color = {0, 0, 255, 255};
    player.physics.velocity.y = velocity;
    engine.audio.play_sound("jumping");
}

///////
// Wall
//////
void Wall::execute(Player &player, Engine &engine)
{
    player.color = {0, 128, 0, 255};
    player.physics.velocity.y = 0;
}

//////////
// In Air
/////////

// Air::Air(double velocity) : velocity{velocity} {}

// void Air::execute(Player& player, Engine& engine) {
//     player.color = {0, 128, 0, 255};
//     player.physics.velocity.y = velocity;
// }
