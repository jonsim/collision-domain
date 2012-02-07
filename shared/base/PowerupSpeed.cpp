/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupSpeed::PowerupSpeed()
{
    mHasSpawned = false;
    mUniqueID = GameCore::mPhysicsCore->getUniqueEntityID();

    mHasBeenCollected = false;
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "SpeedPowerupNode" + boost::lexical_cast<std::string>(mUniqueID));

    mSound = GameCore::mAudioCore->getSoundInstance(POWERUP_SPEED, mUniqueID);
}


/// @brief  Deconstructor.
PowerupSpeed::~PowerupSpeed()
{
   mNode->getParentSceneNode()->removeChild(mNode);
    
    // delete the shape last
    if (mHasSpawned) {
        CollisionShape* collisionShape = mRigidBody->getShape();

        delete mRigidBody;
        delete collisionShape;
    }

    GameCore::mAudioCore->deleteSoundInstance(mSound);
}


/// called when this powerup collides with a player
void PowerupSpeed::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    // play powerup reward sound
    GameCore::mAudioCore->playSoundOrRestart(mSound);

    if (mHasSpawned)
    {
        removeCollideable();
        removeGraphic();
    }

    // apply powerup to player


    // IF THE POWERUP LINGERS LONGER THAN THIS METHOD,
    // isPendingDelete() NEEDS TO RETURN SOMETHING ELSE
}


void PowerupSpeed::frameEvent(const Ogre::FrameEvent& evt)
{
    // no need to rotate the speed powerup
}


bool PowerupSpeed::isPendingDelete()
{
    // the speed powerup cannot be picked up by multiple players
    return mHasBeenCollected;
}


void PowerupSpeed::spawn(Ogre::Vector3 createAboveAt)
{
    if (mHasSpawned) return;
    mHasSpawned = true;

    createGraphic();
    createCollideable();

    mNode->translate(createAboveAt);
    mRigidBody->getBulletRigidBody()->translate(btVector3(createAboveAt.x,createAboveAt.y,createAboveAt.z));
}

void PowerupSpeed::createGraphic()
{
    mEntity = GameCore::mSceneMgr->createEntity("SpeedPowerupMesh" + boost::lexical_cast<std::string>(mUniqueID) , "powerup_speed.mesh");
    mEntity->setMaterialName("powerup_speed");

    int GEOMETRY_QUERY_MASK = 1<<2;
    mEntity->setQueryFlags(GEOMETRY_QUERY_MASK);

#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    mEntity->setCastShadows(false);
    mNode->attachObject(mEntity);

    mNode->scale(0.2f, 0.2f, 0.2f);
}

void PowerupSpeed::createCollideable()
{
    CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
    compoundShape->addChildShape(
        new BoxCollisionShape( Ogre::Vector3( 1.70f, 0.5f, 1.35f ) ),
        Ogre::Vector3(0.0, 0.5f, 0.07f));

        
    mRigidBody = new RigidBody(
            "SpeedPowerup" + boost::lexical_cast<std::string>(mUniqueID),
            GameCore::mPhysicsCore->mWorld,
            COL_POWERUP,
            COL_CAR);

    float bodyRestitution = 1;
    float bodyFriction = 0;
    float bodyMass = 0;

    mRigidBody->setShape(
        mNode,
        compoundShape,
        bodyRestitution,
        bodyFriction,
        bodyMass,
        Ogre::Vector3::ZERO,
        Ogre::Quaternion::IDENTITY);

    // We must set NO CONTACT COLLISIONS to allow cars to drive through the powerups
    mRigidBody->getBulletRigidBody()->setUserPointer(this);
    mRigidBody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mRigidBody->disableDeactivation();
    mRigidBody->showDebugShape(false);
}

void PowerupSpeed::removeGraphic()
{
    mNode->detachObject(mEntity);
}


void PowerupSpeed::removeCollideable()
{
    mRigidBody->getBulletRigidBody()->setUserPointer(NULL);
    GameCore::mPhysicsCore->mWorld->removeObject(mRigidBody);
}
