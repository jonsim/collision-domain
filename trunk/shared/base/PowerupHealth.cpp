/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupHealth::PowerupHealth()
{
    // Local to this class
    mHasSpawned = false;
    mUniqueID = GameCore::mPhysicsCore->getUniqueEntityID();

    // From Powerup
    mHasBeenCollected = false;
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "HealthPowerupNode" + boost::lexical_cast<std::string>(mUniqueID));
}


/// @brief  Deconstructor.
PowerupHealth::~PowerupHealth()
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
void PowerupHealth::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;


    // play powerup reward sound
    GameCore::mAudioCore->playHealthPowerup();


    if (mHasSpawned)
    {
        removeCollideable();
        removeGraphic();
    }
    else
    {
        // create it, to attach it to the player car
        //createGraphic();
    }
    
    // moveGraphicTo(player->attachBonusNode());

    // apply extra health
    player->applyHealthBonus();

    // IF THE POWERUP LINGERS LONGER THAN THIS METHOD,
    // isPendingDelete() NEEDS TO RETURN SOMETHING ELSE
}


void PowerupHealth::frameEvent(const Ogre::FrameEvent& evt)
{
    mNode->rotate( Ogre::Quaternion( 1.0, 0.0, 1.2*(evt.timeSinceLastFrame), 0.0 ) );
}


bool PowerupHealth::isPendingDelete()
{
    return mHasBeenCollected;
}


void PowerupHealth::spawn(Ogre::Vector3 createAboveAt)
{
    if (mHasSpawned) return;
    mHasSpawned = true;

    createGraphic();
    createCollideable();

    mNode->translate(createAboveAt);
}


void PowerupHealth::createGraphic()
{
    entity = GameCore::mSceneMgr->createEntity("HealthPowerupMesh" + boost::lexical_cast<std::string>(mUniqueID) , "powerup_health.mesh");
    entity->setMaterialName("powerup_health");
    
    int GEOMETRY_QUERY_MASK = 1<<2;
    entity->setQueryFlags(GEOMETRY_QUERY_MASK);

#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    entity->setCastShadows(true);
    mNode->attachObject(entity);
    
    mNode->scale(0.2f, 0.2f, 0.2f);
}


void PowerupHealth::createCollideable()
{
    Ogre::Real radius(0.91);
    Ogre::Real height(1.0);
    ConeCollisionShape* coneCollisionShape = new ConeCollisionShape(radius, height, Ogre::Vector3::UNIT_Y);

    Ogre::Vector3 extents(0.95f,0.45f,0.95f);
    CylinderCollisionShape* cylinderCollisionShape = new CylinderCollisionShape(extents, Ogre::Vector3::UNIT_Y);

    CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
    compoundShape->addChildShape(
        coneCollisionShape,
        Ogre::Vector3(0, 0.48, 0),
        Ogre::Quaternion(Ogre::Radian(Ogre::Degree(180)), Ogre::Vector3(0,0,1)));
    compoundShape->addChildShape(
        cylinderCollisionShape,
        Ogre::Vector3(0, 1.425, 0));

    mRigidBody = new RigidBody(
            "HealthPowerup" + boost::lexical_cast<std::string>(mUniqueID),
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


void PowerupHealth::removeGraphic()
{
    mNode->detachObject(entity);
}


void PowerupHealth::removeCollideable()
{
    mRigidBody->getBulletRigidBody()->setUserPointer(NULL);
    GameCore::mPhysicsCore->mWorld->removeObject(mRigidBody);
}
