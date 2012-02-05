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
    PowerupMass(Ogre::Vector3 createAboveAt);
    ~PowerupMass();
    void playerCollision(Player* player);
    void frameEvent( const Ogre::FrameEvent& evt );
    bool isPendingDelete();

private:
};

#endif // #ifndef POWERUPMASS_H
