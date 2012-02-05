/**
 * @file	PowerupSpeed.h
 * @brief 	Random powerup.
 */
#ifndef POWERUPSPEED_H
#define POWERUPSPEED_H

#include "stdafx.h"
#include "SharedIncludes.h"

class Player;
class GameCore;

/**
 *  @brief 	Random powerup
 */
class PowerupSpeed : public Powerup
{
public:
    PowerupSpeed(Ogre::Vector3 createAboveAt);
    ~PowerupSpeed();
    void playerCollision(Player* player);
    void frameEvent( const Ogre::FrameEvent& evt );
    bool isPendingDelete();

private:
};

#endif // #ifndef POWERUPSPEED_H
