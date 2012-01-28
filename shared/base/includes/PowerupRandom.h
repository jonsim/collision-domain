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
class PowerupRandom : Powerup
{
public:
    PowerupRandom();
    ~PowerupRandom();
    void playerCollision(Player* player);

private:
    bool mHasBeenCollected;
    OgreBulletDynamics::RigidBody* mRigidBody;
    Ogre::SceneNode *mNode;
};

#endif // #ifndef POWERUPRANDOM_H
