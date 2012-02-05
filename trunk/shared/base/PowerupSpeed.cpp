/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupSpeed::PowerupSpeed(Ogre::Vector3 createAboveAt)
{
    mHasBeenCollected = false;

    int uniqueID = GameCore::mPhysicsCore->getUniqueEntityID();
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "SpeedPowerupNode" + boost::lexical_cast<std::string>(uniqueID));

    {
        Ogre::Entity *entity = GameCore::mSceneMgr->createEntity("SpeedPowerupMesh" + boost::lexical_cast<std::string>(uniqueID) , "powerup_speed.mesh");
        entity->setMaterialName("powerup_speed");

        int GEOMETRY_QUERY_MASK = 1<<2;
        entity->setQueryFlags(GEOMETRY_QUERY_MASK);

    #if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
        entity->setNormaliseNormals(true);
    #endif // only applicable before shoggoth (1.5.0)

        entity->setCastShadows(false);
        mNode->attachObject(entity);

        mNode->scale(0.2f, 0.2f, 0.2f);
    }

    {
        CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
        compoundShape->addChildShape(
            new BoxCollisionShape( Ogre::Vector3( 1.70f, 0.5f, 1.35f ) ),
            Ogre::Vector3(0.0, 0.5, 0.07));

        
        mRigidBody = new RigidBody(
                "SpeedPowerup" + boost::lexical_cast<std::string>(uniqueID),
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
    
    mNode->translate(createAboveAt);
}


/// @brief  Deconstructor.
PowerupSpeed::~PowerupSpeed()
{
}


/// called when this powerup collides with a player
void PowerupSpeed::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    // play powerup reward sound
    GameCore::mAudioCore->playSpeedPowerup();

    // apply powerup to player

    // remove powerup from map
    // these two lines don't seem to work, I'll finish it on monday.
    //mRigidBody->setVisible(false);
    //GameCore::mPhysicsCore->mWorld->getBulletDynamicsWorld()->removeRigidBody(mRigidBody->getBulletRigidBody());

    // potentially spawn another

}


void PowerupSpeed::frameEvent(const Ogre::FrameEvent& evt)
{
    // no need to rotate the speed powerup
}


bool PowerupSpeed::isPendingDelete()
{
    // the speed powerup can be picked up by multiple players
    return false;
}
