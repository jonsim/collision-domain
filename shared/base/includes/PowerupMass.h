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
    void frameEvent( const Ogre::FrameEvent& evt );
    bool isPendingDelete();
    void spawn(Ogre::Vector3 createAboveAt);

private:
    void createGraphic();
    void createCollideable();
    void removeGraphic();
    void removeCollideable();

    Ogre::Entity *entity;
    bool mHasSpawned;
    int mUniqueID;
};

#endif // #ifndef POWERUPMASS_H
