/**
 * @file	Powerup.h
 * @brief 	Powerups, with callbacks from collisions.
 */
#ifndef POWERUP_H
#define POWERUP_H

#include "stdafx.h"
#include "SharedIncludes.h"

/**
 *  @brief 	Headers for powerup
 */
class Powerup
{
public:
    virtual void playerCollision(Player* player) = 0;
};

#endif // #ifndef POWERUP_H
