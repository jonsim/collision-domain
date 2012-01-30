/**
 * @file	PowerupHealth.h
 * @brief 	Health powerup.
 */
#ifndef POWERUPHEALTH_H
#define POWERUPHEALTH_H

#include "stdafx.h"
#include "SharedIncludes.h"

class Player;
class GameCore;

/**
 *  @brief 	Random powerup
 */
class PowerupHealth : public Powerup
{
public:
    PowerupHealth();
    ~PowerupHealth();
    void playerCollision(Player* player);

private:

};

#endif // #ifndef POWERUPHEALTH_H
