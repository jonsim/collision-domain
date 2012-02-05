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
    virtual void frameEvent( const Ogre::FrameEvent& evt ) = 0;
    virtual bool isPendingDelete() = 0;

    //int getPowerupType(); if you have to check this, its probably bad oo design
    //void setPowerupType( int iType ); http://stackoverflow.com/questions/500493/c-equivalent-of-instanceof
    void hide();

protected:
    //int mType;  
    OgreBulletDynamics::RigidBody* mRigidBody;
    Ogre::SceneNode *mNode;

    bool mHasBeenCollected;
};

#endif // #ifndef POWERUP_H
