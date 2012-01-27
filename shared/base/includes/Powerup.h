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
    Powerup();
    ~Powerup();
    virtual void playerCollision(Player* player);
};

#endif // #ifndef POWERUP_H
