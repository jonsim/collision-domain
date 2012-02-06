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
    PowerupSpeed();
    ~PowerupSpeed();
    void playerCollision(Player* player);
    void frameEvent( const Ogre::FrameEvent& evt );
    bool isPendingDelete();
    void spawn(Ogre::Vector3 createAboveAt);
    
private:
    void createGraphic();
    void createCollideable();
    void removeGraphic();
    void removeCollideable();

    Ogre::Entity *mEntity;
    bool mHasSpawned;
    int mUniqueID;
};

#endif // #ifndef POWERUPSPEED_H
