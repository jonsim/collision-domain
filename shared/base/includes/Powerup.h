/**
 * @file	Powerup.h
 * @brief 	Powerups, with callbacks from collisions.
 */
#ifndef POWERUP_H
#define POWERUP_H

#include "stdafx.h"
#include "Player.h"
#include "OgreOggSound.h"

using namespace OgreOggSound;

/**
 *  @brief 	Headers for powerup
 */
class Powerup
{
public:
    Powerup(Ogre::Vector3 spawnAt, int poolIndex);
    Powerup(PowerupType powerupType, Ogre::Vector3 spawnAt, int poolIndex);
    ~Powerup();

#ifdef COLLISION_DOMAIN_SERVER
    void playerCollision(Player* player);
#else
    void playerCollision(Player* player, float extraData);
#endif
    void frameEvent(const float timeSinceLastFrame);
    bool isPendingDelete();

    PowerupType getType();
    int  getIndex();

    Ogre::Vector3 getPosition();
    btQuaternion getRotation();

    Ogre::OverlayElement* getBigScreenOverlayElement();
    void reinitBigScreenOverlayElementIfNull();

private:
    bool mHasBeenCollected;
    OgreOggISound *mSound;

    PowerupType mPowerupType;
    int mPoolIndex;
    int mUniqueID;

    Ogre::SceneNode *mNode;
    Ogre::OverlayElement* mBigScreenOverlayElement;

    // Used in the server only
    btRigidBody *mRigidBody;
};

#endif // #ifndef POWERUP_H
