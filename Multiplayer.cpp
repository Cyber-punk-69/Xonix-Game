#include "Multiplayer.h"

MultiplayerManager::MultiplayerManager()
{
    resetPlayers();
    sharedTimer = 0.0f;
    p1FrozenByP2 = false; p2FrozenByP1 = false;
    p1FreezeTimeLeft = p2FreezeTimeLeft = 0.0f;
}

void MultiplayerManager::resetPlayers()
{
    p1.reset(10, 0);
    p1.invIndex = 0;
    p2.reset(29, 0); // place second player near right side
    p2.invIndex = 1;
}

void MultiplayerManager::update(float dt)
{
    if (p1FrozenByP2) {
        p1FreezeTimeLeft -= dt;
        if (p1FreezeTimeLeft <= 0.0f) { p1FrozenByP2 = false; p1FreezeTimeLeft = 0.0f; }
    }
    if (p2FrozenByP1) {
        p2FreezeTimeLeft -= dt;
        if (p2FreezeTimeLeft <= 0.0f) { p2FrozenByP1 = false; p2FreezeTimeLeft = 0.0f; }
    }
}

void MultiplayerManager::applyPlayerPowerup(int playerIndex)
{
    // when player uses powerup it should freeze enemies and the other player for 3 seconds
    if (playerIndex == 0) {
        p2FrozenByP1 = true; p2FreezeTimeLeft = 3.0f;
    } else {
        p1FrozenByP2 = true; p1FreezeTimeLeft = 3.0f;
    }
}
