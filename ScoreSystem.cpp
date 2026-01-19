#include "ScoreSystem.h"
#include <cmath>

// Allowed standard headers only in this project context

ScoreSystem::ScoreSystem()
{
    reset(false);
}

void ScoreSystem::reset(bool keepInventory)
{
    score = 0;
    rewardCount = 0;
    doubleThreshold = 11; // >10 tiles => double (so threshold is 11)

    if (!keepInventory) {
        powerupInventory = 0;
    }
    // Always reset nextPowerupScore to start-of-run threshold so awards are based on current run score
    nextPowerupScore = 50; // first powerup at 50

    enemiesFrozen = false;
    frozenTimeLeft = 0.0f;
}

int ScoreSystem::getScore() const { return score; }
int ScoreSystem::getInventory() const { return powerupInventory; }
bool ScoreSystem::isEnemiesFrozen() const { return enemiesFrozen; }
int ScoreSystem::getRewardCount() const { return rewardCount; }

void ScoreSystem::setInventory(int inv)
{
    if (inv < 0) inv = 0;
    if (inv > 100) inv = 100;
    powerupInventory = inv;
}

void ScoreSystem::setScore(int s)
{
    if (s < 0) s = 0;
    score = s;
    // update nextPowerupScore based on current score
    if (score >= 130) {
        nextPowerupScore = ((score - 130) / 30 + 1) * 30 + 130;
    } else if (score >= 100) {
        nextPowerupScore = 130;
    } else if (score >= 70) {
        nextPowerupScore = 100;
    } else if (score >= 50) {
        nextPowerupScore = 70;
    } else {
        nextPowerupScore = 50;
    }
}

void ScoreSystem::setRewardCount(int rc)
{
    if (rc < 0) rc = 0;
    rewardCount = rc;
    // update threshold based on reward count
    if (rewardCount >= 5) {
        doubleThreshold = 6;
    } else if (rewardCount >= 3) {
        doubleThreshold = 6;
    } else {
        doubleThreshold = 11;
    }
}

int ScoreSystem::registerCapture(int tilesCaptured)
{
    if (tilesCaptured <= 0) return 0;

    int points = tilesCaptured;
    int multiplier = 1;

    // Determine thresholds based on rewardCount
    // initial: more than 10 tiles -> x2
    // after 3 occurrences: threshold reduces to more than 5 tiles for double
    // after 5 occurrences: capturing more than 5 tiles yields x4

    if (rewardCount >= 5) {
        if (tilesCaptured > 5) {
            multiplier = 4;
        } else if (tilesCaptured > 10) {
            multiplier = 2; // fallback: very large captures still give at least x2
        }
    } else if (rewardCount >= 3) {
        if (tilesCaptured > 5) multiplier = 2;
    } else {
        if (tilesCaptured > 10) multiplier = 2;
    }

    points *= multiplier;

    if (multiplier > 1) {
        rewardCount++;
        // adjust internal threshold helper
        if (rewardCount >= 3) doubleThreshold = 6; // >5 becomes threshold
        if (rewardCount >= 5) doubleThreshold = 6; // ensure consistent
    }

    score += points;

    // check whether score crossed powerup award thresholds
    checkPowerupAward();

    return points;
}

void ScoreSystem::checkPowerupAward()
{
    // Award powerups at specific scores: 50,70,100,130 then every +30
    while (score >= nextPowerupScore) {
        if (powerupInventory >= 100) {
            // cap inventory at 100 and stop awarding
            break;
        }
        powerupInventory++;
        // compute next threshold
        if (nextPowerupScore < 70) nextPowerupScore = 70;
        else if (nextPowerupScore == 70) nextPowerupScore = 100;
        else if (nextPowerupScore == 100) nextPowerupScore = 130;
        else nextPowerupScore += 30;
    }
}

bool ScoreSystem::usePowerup()
{
    if (powerupInventory <= 0) return false;
    powerupInventory--;
    enemiesFrozen = true;
    frozenTimeLeft = 3.0f; // freeze for 3 seconds
    return true;
}

void ScoreSystem::update(float dt)
{
    if (enemiesFrozen) {
        frozenTimeLeft -= dt;
        if (frozenTimeLeft <= 0.0f) {
            enemiesFrozen = false;
            frozenTimeLeft = 0.0f;
        }
    }
}
