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
    virtual void playerCollision(Player* player, PowerupType pwrType) {} // For client removing randoms
    virtual void frameEvent( const float timeSinceLastFrame ) = 0;
    virtual bool isPendingDelete() = 0;

    int  getIndex() { return mPoolIndex; }
    void setIndex( int x ) { mPoolIndex = x; }

    Ogre::Vector3 getPosition() { if( mNode ) return mNode->getPosition(); return Ogre::Vector3(); }

    //int getPowerupType(); if you have to check this, its probably bad oo design
    //void setPowerupType( int iType ); http://stackoverflow.com/questions/500493/c-equivalent-of-instanceof

protected:
    btRigidBody     *mRigidBody;
    Ogre::SceneNode *mNode;

    bool            mHasBeenCollected;
    OgreOggISound   *mSound;

    int             mPoolIndex;
};

#endif // #ifndef POWERUP_H
