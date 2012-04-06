/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"


/// @brief  Constructor.
PowerupMass::PowerupMass()
{
    mHasSpawned = false;
    mUniqueID = GameCore::mPhysicsCore->getUniqueEntityID();

    mHasBeenCollected = false;
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("MassPowerupNode" + boost::lexical_cast<std::string>(mUniqueID));

#ifdef COLLISION_DOMAIN_CLIENT
    mSound = GameCore::mAudioCore->getSoundInstance(POWERUP_MASS, mUniqueID);
#endif
}


/// @brief  Deconstructor.
PowerupMass::~PowerupMass()
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
    
#ifdef COLLISION_DOMAIN_CLIENT
    GameCore::mAudioCore->deleteSoundInstance(mSound);
#endif
}


/// called when this powerup collides with a player
void PowerupMass::playerCollision(Player* player)
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
#ifdef COLLISION_DOMAIN_CLIENT
        GameCore::mAudioCore->playSoundOrRestart(mSound);
#endif
        // apply powerup to player
        //btVector3 inertia;
        //player->getCar()->getVehicle()->getRigidBody()->getCollisionShape()->calculateLocalInertia( 20000, inertia );
        //player->getCar()->getVehicle()->getRigidBody()->setMassProps( 20000, inertia );
    }

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
    Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("MassPowerupMesh" + boost::lexical_cast<std::string>(mUniqueID) , "powerup_mass.mesh");

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
    btVector3 halfExtents(1.28f, 0.70f, 1.28f);
    btCylinderShape *cylinderCollisionShape = new btCylinderShape( halfExtents );

    btCompoundShape *compoundShape = new btCompoundShape();
    btTransform localTrans( btQuaternion::getIdentity(), btVector3( 0, 0.69f, 0 ) );
    compoundShape->addChildShape( localTrans, cylinderCollisionShape );

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

void PowerupMass::spawn(Ogre::Vector3 createAboveAt)
{
    if (mHasSpawned) return;
    mHasSpawned = true;

#ifdef COLLISION_DOMAIN_CLIENT
    createGraphic();
#endif
    createCollideable();

    mNode->translate(createAboveAt);
    mRigidBody->translate(btVector3(createAboveAt.x,createAboveAt.y,createAboveAt.z));
}

void PowerupMass::removeGraphic()
{
    mNode->detachAllObjects();
}


void PowerupMass::removeCollideable()
{
    mRigidBody->setUserPointer(NULL);
    GameCore::mPhysicsCore->removeBody( mRigidBody );
}
