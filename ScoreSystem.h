#ifndef SCORESYSTEM_H
#define SCORESYSTEM_H

#include <string>

class ScoreSystem
{
public:
    ScoreSystem();

    // register how many tiles were captured in one closure move
    // returns points awarded for this capture
    int registerCapture(int tilesCaptured);

    // try to use one power-up (starts enemy freeze for configured duration)
    // returns true if used
    bool usePowerup();

    // update timers (dt in seconds)
    void update(float dt);

    // reset state for a new game; if keepInventory==true, do not clear stored power-ups
    void reset(bool keepInventory = false);

    // accessors
    int getScore() const;
    int getInventory() const;
    bool isEnemiesFrozen() const;
    int getRewardCount() const;

    // allow external code to set inventory (used when loading saved per-user inventory)
    void setInventory(int inv);
    
    // allow external code to set score and reward count (used when loading saved game)
    void setScore(int s);
    void setRewardCount(int rc);

private:
    int score;
    int rewardCount;     // how many times bonus has been earned
    int doubleThreshold; // current threshold for double bonus

    int powerupInventory;
    int nextPowerupScore; // next score at which a powerup is granted

    bool enemiesFrozen;
    float frozenTimeLeft;

    void checkPowerupAward();
};

#endif // SCORESYSTEM_H
