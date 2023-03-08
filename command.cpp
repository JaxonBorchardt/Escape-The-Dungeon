#include "command.h"

#include "player.h"
#include "world.h"

////////
// Stop
///////

void Stop::execute(Player& player, World& world) {
    player.color = {255, 0, 0, 255};  // green for stop
    // player.physics.velocity.y = 0;
    player.physics.acceleration.x = 0;
}

////////
// Running
///////

Run::Run(double acceleration) : acceleration{acceleration} {}

void Run::execute(Player& player, World& world) {
    player.color = {255, 255, 0, 255};  // yellow for running
    player.physics.acceleration.x = acceleration;
}

////////
// Jump
///////

Jump::Jump(double velocity) : velocity{velocity} {}

void Jump::execute(Player& player, World& world) {
    player.color = {0, 0, 255, 255};
    player.physics.velocity.y = velocity;
}