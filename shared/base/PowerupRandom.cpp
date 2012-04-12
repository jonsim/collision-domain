/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"


/// @brief  Constructor.
PowerupRandom::PowerupRandom(Ogre::Vector3 createAboveAt)
{
    mHasBeenCollected = false;

    // Create the powerup node
    int uniqueID = GameCore::mPhysicsCore->getUniqueEntityID();
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("RandomPowerupNode" + boost::lexical_cast<std::string>(uniqueID));
    mNode->scale(0.2f, 0.2f, 0.2f);

    // Create the powerup graphics
#ifdef COLLISION_DOMAIN_CLIENT
    Ogre::Entity* pupEntity = GameCore::mSceneMgr->createEntity("Powerup" + boost::lexical_cast<std::string>(uniqueID) , "powerup_random.mesh");
    //int GEOMETRY_QUERY_MASK = 1<<2;
    //entity->setQueryFlags(GEOMETRY_QUERY_MASK);
    pupEntity->setCastShadows(true);
    mNode->attachObject(pupEntity);
#endif

    // Create the powerup physics
    //Ogre::Vector3 axis = Ogre::Vector3::UNIT_Z;
    //Ogre::Vector3 halfExtents(1, 1, 0.125);
    //CylinderCollisionShape* collisionShape = new CylinderCollisionShape(halfExtents, axis);
    float bodyRestitution = 1;
    float bodyFriction = 0;
    float bodyMass = 0;
    btVector3 inertia;

    btCompoundShape* compoundShape = new btCompoundShape();
    btTransform localTrans( btQuaternion::getIdentity(), btVector3( 0.0f, 0.9f, 0.0f ) );
    compoundShape->addChildShape( localTrans, new btSphereShape( Ogre::Real(1.0) ) );
    compoundShape->calculateLocalInertia( bodyMass, inertia );
    BtOgre::RigidBodyState* state = new BtOgre::RigidBodyState( mNode );

    mRigidBody = new btRigidBody( bodyMass, state, compoundShape, inertia );
    mRigidBody->setRestitution( bodyRestitution );
    mRigidBody->setFriction( bodyFriction );

    GameCore::mPhysicsCore->addRigidBody( mRigidBody, COL_POWERUP, COL_CAR );

    // We must set NO CONTACT COLLISIONS to allow cars to drive through the powerups
    mRigidBody->setUserPointer(this);
    mRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    mRigidBody->setActivationState( DISABLE_DEACTIVATION );
    
    mNode->translate( createAboveAt );
    mRigidBody->translate(BtOgre::Convert::toBullet(createAboveAt));

#ifdef COLLISION_DOMAIN_CLIENT
    // Add the sound
    mSound = GameCore::mAudioCore->getSoundInstance(POWERUP_RANDOM, uniqueID, NULL);
#endif
}


/// @brief  Deconstructor.
PowerupRandom::~PowerupRandom()
{
    mNode->getParentSceneNode()->removeChild(mNode);
    
    // delete the shape last
    btCollisionShape* collisionShape = mRigidBody->getCollisionShape();

    if( mRigidBody->getMotionState() )
        delete mRigidBody->getMotionState();

    GameCore::mPhysicsCore->getWorld()->removeRigidBody( mRigidBody );
        //GameCore::mPhysicsCore->getWorld()->removeCollisionObject( mRigidBody );

    delete mRigidBody;
    delete collisionShape;
    
#ifdef COLLISION_DOMAIN_CLIENT
    GameCore::mAudioCore->deleteSoundInstance(mSound);
#endif
}


/// called when this powerup collides with a player
void PowerupRandom::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    if( player != NULL )
    {
#ifdef COLLISION_DOMAIN_CLIENT
        GameCore::mAudioCore->playSoundOrRestart(mSound);
#endif

        Powerup* powerup = NULL;
        {
            // only a few of the powerup types fit as "random" rewards so here they are:
            PowerupType mPotentialPowerups[3] = {POWERUP_HEALTH, POWERUP_MASS, POWERUP_SPEED};

            PowerupType chosen = mPotentialPowerups[
                rand() % (sizeof(mPotentialPowerups) / sizeof(PowerupType))];
            powerup = GameCore::mPowerupPool->createPowerup(chosen);

#ifdef COLLISION_DOMAIN_SERVER
            GameCore::mNetworkCore->sendPowerupCollect( getIndex(), player, chosen );
#endif
        }

        if (powerup) powerup->playerCollision(player);
    }

    removeFromWorlds();

    // IF THE POWERUP LINGERS LONGER THAN THIS METHOD,
    // isPendingDelete() NEEDS TO RETURN SOMETHING ELSE
}

void PowerupRandom::frameEvent(const float timeSinceLastFrame)
{

}

void PowerupRandom::playerCollision(Player* player, PowerupType pwrType)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    if( player != NULL )
    {
#ifdef COLLISION_DOMAIN_CLIENT
        GameCore::mAudioCore->playSoundOrRestart(mSound);
#endif

        Powerup* powerup = NULL;
        {
            powerup = GameCore::mPowerupPool->createPowerup(pwrType);
        }

        if (powerup) powerup->playerCollision(player);
    }

    removeFromWorlds();

    // IF THE POWERUP LINGERS LONGER THAN THIS METHOD,
    // isPendingDelete() NEEDS TO RETURN SOMETHING ELSE
}


bool PowerupRandom::isPendingDelete()
{
    return mHasBeenCollected;
}


void PowerupRandom::removeFromWorlds()
{
    mRigidBody->setUserPointer(NULL);
    GameCore::mPhysicsCore->removeBody( mRigidBody );
    mNode->detachAllObjects();
}
