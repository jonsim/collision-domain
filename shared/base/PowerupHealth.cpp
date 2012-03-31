/**
 * @file    PowerupRandom.cpp
 * @brief     Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"


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

    mSound = GameCore::mAudioCore->getSoundInstance(POWERUP_HEALTH, mUniqueID);
}


/// @brief  Deconstructor.
PowerupHealth::~PowerupHealth()
{
   mNode->getParentSceneNode()->removeChild(mNode);

    // delete the shape last
    if (mHasSpawned) {
        btCollisionShape* collisionShape = mRigidBody->getCollisionShape();

        if( mRigidBody->getMotionState() )
            delete mRigidBody->getMotionState();

        GameCore::mPhysicsCore->getWorld()->removeRigidBody( mRigidBody );
        //GameCore::mPhysicsCore->getWorld()->removeCollisionObject( mRigidBody );

        delete mRigidBody;
        delete collisionShape;
    }

    GameCore::mAudioCore->deleteSoundInstance(mSound);
}


/// called when this powerup collides with a player
void PowerupHealth::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
        mHasBeenCollected = true;

    if (mHasSpawned)
    {
        removeCollideable();
        removeGraphic();
    }

    if( player != NULL )
    {
        // play powerup reward sound
        GameCore::mAudioCore->playSoundOrRestart(mSound);

        // apply extra health
        player->applyHealthBonus();
    }

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
    
    mNode->translate( createAboveAt );
    mRigidBody->translate(btVector3(createAboveAt.x,createAboveAt.y,createAboveAt.z));
}


void PowerupHealth::createGraphic()
{
    entity = GameCore::mSceneMgr->createEntity("HealthPowerupMesh" + boost::lexical_cast<std::string>(mUniqueID) , "powerup_health.mesh");
    
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
    btCompoundShape *compoundShape = new btCompoundShape();

    Ogre::Real radius(0.91f);
    Ogre::Real height(1.0f);
    btConeShape *coneCollisionShape = new btConeShape( radius, height );
    btTransform coneTrans( btQuaternion( btVector3( 0, 0, 1 ), btRadians( btDegrees( 180 ) ) ), btVector3( 0, 0.48f, 0 ) );
    compoundShape->addChildShape( coneTrans, coneCollisionShape );

    btVector3 extents(0.95f,0.45f,0.95f);
    btCylinderShape *cylinderCollisionShape = new btCylinderShape( extents );
    btTransform cylinderTrans( btQuaternion::getIdentity(), btVector3( 0, 1.425f, 0 ) );
    compoundShape->addChildShape( cylinderTrans, cylinderCollisionShape );

    float bodyRestitution = 1;
    float bodyFriction = 0;
    float bodyMass = 0;

    btVector3 inertia;
    compoundShape->calculateLocalInertia( bodyMass, inertia );

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState( mNode );

    mRigidBody = new btRigidBody( bodyMass, state, compoundShape, inertia );

    mRigidBody->setRestitution( bodyRestitution );
    mRigidBody->setFriction( bodyFriction );

    GameCore::mPhysicsCore->addRigidBody( mRigidBody, COL_POWERUP, COL_CAR );
        
    // We must set NO CONTACT COLLISIONS to allow cars to drive through the powerups
    mRigidBody->setUserPointer(this);
    mRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mRigidBody->setActivationState( DISABLE_DEACTIVATION );   
}


void PowerupHealth::removeGraphic()
{
    mNode->detachObject(entity);
}


void PowerupHealth::removeCollideable()
{
    mRigidBody->setUserPointer(NULL);
    GameCore::mPhysicsCore->removeBody( mRigidBody );
}
