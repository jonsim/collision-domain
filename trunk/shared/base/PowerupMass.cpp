/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupMass::PowerupMass()
{
    mHasSpawned = false;
    mUniqueID = GameCore::mPhysicsCore->getUniqueEntityID();

    mHasBeenCollected = false;
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "MassPowerupNode" + boost::lexical_cast<std::string>(mUniqueID));
}


/// @brief  Deconstructor.
PowerupMass::~PowerupMass()
{
   mNode->getParentSceneNode()->removeChild(mNode);
    
    // delete the shape last
    if (mHasSpawned) {
        CollisionShape* collisionShape = mRigidBody->getShape();

        delete mRigidBody;
        delete collisionShape;
    }
}


/// called when this powerup collides with a player
void PowerupMass::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    //GameCore::mAudioCore->playHealthPowerup();
    
    if (mHasSpawned)
    {
        removeCollideable();
        removeGraphic();
    }

    // apply powerup to player

    // IF THE POWERUP LINGERS LONGER THAN THIS METHOD,
    // isPendingDelete() NEEDS TO RETURN SOMETHING ELSE
}


void PowerupMass::frameEvent(const Ogre::FrameEvent& evt)
{
    mNode->rotate( Ogre::Quaternion( 1.0, 0.0, 1.2*(evt.timeSinceLastFrame), 0.0 ) );
}


bool PowerupMass::isPendingDelete()
{
    return mHasBeenCollected;
}

void PowerupMass::createGraphic()
{
    entity = GameCore::mSceneMgr->createEntity("MassPowerupMesh" + boost::lexical_cast<std::string>(mUniqueID) , "powerup_mass.mesh");
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

void PowerupMass::createCollideable()
{
    Ogre::Vector3 axis = Ogre::Vector3::UNIT_Y;
    Ogre::Vector3 halfExtents(1.28f, 0.70f, 1.28f);
    CylinderCollisionShape* cylinderCollisionShape = new CylinderCollisionShape(halfExtents, axis);

    CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
    compoundShape->addChildShape(
        cylinderCollisionShape,
        Ogre::Vector3(0, 0.69f, 0));
        
    mRigidBody = new RigidBody(
            "MassPowerup" + boost::lexical_cast<std::string>(mUniqueID),
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

void PowerupMass::spawn(Ogre::Vector3 createAboveAt)
{
    if (mHasSpawned) return;
    mHasSpawned = true;

    createGraphic();
    createCollideable();

    mNode->translate(createAboveAt);
    mRigidBody->getBulletRigidBody()->translate(btVector3(createAboveAt.x,createAboveAt.y,createAboveAt.z));
}

void PowerupMass::removeGraphic()
{
    mNode->detachObject(entity);
}


void PowerupMass::removeCollideable()
{
    mRigidBody->getBulletRigidBody()->setUserPointer(NULL);
    GameCore::mPhysicsCore->mWorld->removeObject(mRigidBody);
}
