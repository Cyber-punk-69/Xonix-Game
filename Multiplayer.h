#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "Player.h"

struct MultiplayerManager {
    PlayerState p1;
    PlayerState p2;

    float sharedTimer;

    // freeze flag for player-to-player powerups
    bool p1FrozenByP2;
    bool p2FrozenByP1;
    float p1FreezeTimeLeft;
    float p2FreezeTimeLeft;

    MultiplayerManager();
    void resetPlayers();

    // update freeze timers
    void update(float dt);

    // apply powerup effect from player idx (0 or 1)
    void applyPlayerPowerup(int playerIndex);
};

#endif // MULTIPLAYER_H
