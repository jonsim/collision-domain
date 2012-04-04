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

    int uniqueID = GameCore::mPhysicsCore->getUniqueEntityID();
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "RandomPowerupNode" + boost::lexical_cast<std::string>(uniqueID));

    {
        entity = GameCore::mSceneMgr->createEntity("RandomPowerupMesh" + boost::lexical_cast<std::string>(uniqueID) , "powerup_random.mesh");

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
        //Ogre::Vector3 axis = Ogre::Vector3::UNIT_Z;
        //Ogre::Vector3 halfExtents(1, 1, 0.125);
        //CylinderCollisionShape* collisionShape = new CylinderCollisionShape(halfExtents, axis);
        btCompoundShape *compoundShape = new btCompoundShape();
        btTransform localTrans( btQuaternion::getIdentity(), btVector3( 0.0f, 0.9f, 0.0f ) );
        compoundShape->addChildShape( localTrans, new btSphereShape( Ogre::Real(1.0) ) );
        
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
    
    mNode->translate( createAboveAt );
    mRigidBody->translate(BtOgre::Convert::toBullet(createAboveAt));

    mSound = GameCore::mAudioCore->getSoundInstance(POWERUP_RANDOM, uniqueID, NULL);
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

    GameCore::mAudioCore->deleteSoundInstance(mSound);
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
        GameCore::mAudioCore->playSoundOrRestart(mSound);

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

void PowerupRandom::playerCollision(Player* player, PowerupType pwrType)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    if( player != NULL )
    {
        GameCore::mAudioCore->playSoundOrRestart(mSound);

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

void PowerupRandom::frameEvent(const Ogre::FrameEvent& evt)
{
    mNode->rotate( Ogre::Quaternion( 1.0, 0.0, 1.2*(evt.timeSinceLastFrame), 0.0 ) );
}


bool PowerupRandom::isPendingDelete()
{
    return mHasBeenCollected;
}


void PowerupRandom::removeFromWorlds()
{
    mRigidBody->setUserPointer(NULL);
    GameCore::mPhysicsCore->removeBody( mRigidBody );
    mNode->detachObject(entity);
}
