/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupMass::PowerupMass(Ogre::Vector3 createAboveAt)
{
    mHasBeenCollected = false;

    int uniqueID = GameCore::mPhysicsCore->getUniqueEntityID();
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "MassPowerupNode" + boost::lexical_cast<std::string>(uniqueID));

    {
        Ogre::Entity *entity = GameCore::mSceneMgr->createEntity("MassPowerupMesh" + boost::lexical_cast<std::string>(uniqueID) , "powerup_mass.mesh");
        //entity->setMaterialName("powerup_mass_weight");

        int GEOMETRY_QUERY_MASK = 1<<2;
        entity->setQueryFlags(GEOMETRY_QUERY_MASK);

    #if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
        entity->setNormaliseNormals(true);
    #endif // only applicable before shoggoth (1.5.0)

        entity->setCastShadows(true);
        mNode->attachObject(entity);

        mNode->scale(0.2f, 0.2f, 0.2f);
    }

    {
        Ogre::Vector3 axis = Ogre::Vector3::UNIT_Y;
        Ogre::Vector3 halfExtents(1.28, 0.70, 1.28);
        CylinderCollisionShape* cylinderCollisionShape = new CylinderCollisionShape(halfExtents, axis);

        CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
        compoundShape->addChildShape(
            cylinderCollisionShape,
            Ogre::Vector3(0, 0.69, 0));
        
        mRigidBody = new RigidBody(
                "MassPowerup" + boost::lexical_cast<std::string>(uniqueID),
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
PowerupMass::~PowerupMass()
{
}


/// called when this powerup collides with a player
void PowerupMass::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    // play powerup reward sound
    //GameCore::mAudioCore->playHealthPowerup();

    // apply powerup to player

    // remove powerup from map
    //mNode->setDebugDisplayEnabled(false);
    //mNode->setVisible(false);
    // these two lines don't seem to work, I'll finish it on monday.
    //mRigidBody->setVisible(false);
    //GameCore::mPhysicsCore->mWorld->getBulletDynamicsWorld()->removeRigidBody(mRigidBody->getBulletRigidBody());

    // potentially spawn another
    mRigidBody->getBulletRigidBody()->setUserPointer(NULL);
}


void PowerupMass::frameEvent(const Ogre::FrameEvent& evt)
{
    mNode->rotate( Ogre::Quaternion( 1.0, 0.0, 1.2*(evt.timeSinceLastFrame), 0.0 ) );
}


bool PowerupMass::isPendingDelete()
{
    return false; // temp fix for crash. make sure this object is never deleted
    //return mHasBeenCollected;
}
