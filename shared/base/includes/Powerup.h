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
    friend class PowerupPool;

public:
    virtual void playerCollision(Player* player) = 0;

    int getPowerupType() { return mType; }
    void setPowerupType( int iType ) { mType = iType; }

    void hide() { mNode->setDebugDisplayEnabled( false ); mNode->setVisible(false); }

    bool mHasBeenCollected;
protected:
    int mType;  
    OgreBulletDynamics::RigidBody* mRigidBody;
    Ogre::SceneNode *mNode;
};

#endif // #ifndef POWERUP_H
