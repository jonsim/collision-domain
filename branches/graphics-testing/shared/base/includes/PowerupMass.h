/**
 * @file	PowerupMass.h
 * @brief 	Random powerup.
 */
#ifndef POWERUPMASS_H
#define POWERUPMASS_H

#include "stdafx.h"
#include "SharedIncludes.h"

class Player;
class GameCore;

/**
 *  @brief 	Random powerup
 */
class PowerupMass : public Powerup
{
public:
    PowerupMass();
    ~PowerupMass();
    void playerCollision(Player* player);

private:
};

#endif // #ifndef POWERUPMASS_H
