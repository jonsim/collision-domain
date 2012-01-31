/**
 * @file	PowerupRandom.h
 * @brief 	Random powerup.
 */
#ifndef POWERUPRANDOM_H
#define POWERUPRANDOM_H

#include "stdafx.h"
#include "SharedIncludes.h"

class Player;
class GameCore;

/**
 *  @brief 	Random powerup
 */
class PowerupRandom : public Powerup
{
public:
    PowerupRandom();
    ~PowerupRandom();
    void playerCollision(Player* player);

private:
};

#endif // #ifndef POWERUPRANDOM_H
