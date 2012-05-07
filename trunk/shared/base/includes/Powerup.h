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

enum PowerupType
{
	POWERUP_HEALTH,
	POWERUP_MASS,
	POWERUP_SPEED,
	POWERUP_COUNT
};

/**
 *  @brief 	Headers for powerup
 */
class Powerup
{
public:
    Powerup(Ogre::Vector3 spawnAt, int poolIndex);
    Powerup(PowerupType powerupType, Ogre::Vector3 spawnAt, int poolIndex);
    ~Powerup();

    void playerCollision(Player* player);
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
