/**
 * @file	AudioCore.cpp
 * @brief 	An interface for firing off sounds
 */

#include "stdafx.h"
#include "SharedIncludes.h"

Powerup::Powerup(Ogre::Vector3 spawnAt, int poolIndex)
{
    Powerup((PowerupType)(rand() % POWERUP_COUNT), spawnAt, poolIndex);
}

Powerup::Powerup(PowerupType powerupType, Ogre::Vector3 spawnAt, int poolIndex)
    : mHasBeenCollected(false),
      mPowerupType(powerupType),
      mPoolIndex(poolIndex),
      mUniqueID(GameCore::mPhysicsCore->getUniqueEntityID()),
      position(spawnAt),
      mSound(NULL),
      mNode(NULL),
      mBigScreenOverlayElement(NULL),
      mRigidBody(NULL)
{
    #ifdef COLLISION_DOMAIN_CLIENT
        mSound = GameCore::mAudioCore->getSoundInstance(powerupType, mUniqueID, NULL);

        mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("PowerupNode" + boost::lexical_cast<std::string>(mUniqueID));
        PhysicsCore::auto_scale_scenenode(mNode);

        Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("PowerupEntity" + boost::lexical_cast<std::string>(mUniqueID) , "crate.mesh");
        entity->setCastShadows(true);
        entity->setMaterialName("crate_uv");

        mNode->attachObject(entity);
        mNode->translate(spawnAt);

        if ( GameCore::mClientGraphics->mBigScreen != NULL )
            mBigScreenOverlayElement = GameCore::mClientGraphics->mBigScreen->createPowerupOverlayElement(spawnAt, mUniqueID);
    #endif
    
    #ifndef COLLISION_DOMAIN_CLIENT // SERVER
        //mRigidBody = ;
    #endif
}

Powerup::~Powerup()
{
    #ifdef COLLISION_DOMAIN_CLIENT
        GameCore::mAudioCore->deleteSoundInstance(mSound);
        
        mNode->detachAllObjects();
        mNode->getParentSceneNode()->removeChild(mNode);

        // delete the big screen overlay element?
    #endif
    
    #ifndef COLLISION_DOMAIN_CLIENT // SERVER
        // delete the shape last
        /*
        mRigidBody->setUserPointer(NULL);
        btCollisionShape* collisionShape = mRigidBody->getCollisionShape();
        GameCore::mPhysicsCore->removeBody( mRigidBody );
        delete collisionShape;
        */
    #endif
}

// player can be NULL and if it is, silently remove the powerup
void Powerup::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    #ifdef COLLISION_DOMAIN_CLIENT
        if (player != NULL)
            GameCore::mAudioCore->playSoundOrRestart(mSound);
        
        // remove mBigScreenOverlayElement from minimap
    #endif
    
    #ifndef COLLISION_DOMAIN_CLIENT // SERVER
        if (player != NULL)
            switch (mPowerupType)
            {
            case POWERUP_HEALTH:
                break;
            case POWERUP_MASS:
                break;
            case POWERUP_SPEED:
                break;
            default:
                break;
            }

        // don't delete this yet as it could be in the middle of a timestep
        mRigidBody->setUserPointer(NULL);
    #endif
}

void Powerup::frameEvent(const float timeSinceLastFrame)
{

}

bool Powerup::isPendingDelete()
{
    return mHasBeenCollected;
}

PowerupType Powerup::getType()
{
    return mPowerupType;
}

int Powerup::getIndex()
{
    return mPoolIndex;
}

Ogre::Vector3 Powerup::getPosition()
{
    return position;
}

Ogre::OverlayElement* Powerup::getBigScreenOverlayElement()
{
    return mBigScreenOverlayElement;
}
