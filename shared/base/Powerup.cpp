/**
 * @file        AudioCore.cpp
 * @brief       An interface for firing off sounds
 */

#include "stdafx.h"
#include "Powerup.h"
#include "GameCore.h"

Powerup::Powerup(Ogre::Vector3 spawnAt, int poolIndex)
{
    Powerup((PowerupType)(rand() % POWERUP_COUNT), spawnAt, poolIndex);
}

Powerup::Powerup(PowerupType powerupType, Ogre::Vector3 spawnAt, int poolIndex)
    : mHasBeenCollected(false),
      mPowerupType(powerupType),
      mPoolIndex(poolIndex),
      mUniqueID(GameCore::mPhysicsCore->getUniqueEntityID()),
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
    
    // Add the physics box for both client and server. Make the client's not collide with anyone
    
    btScalar bodyRestitution = 0.3f;
    btScalar bodyFriction = 1.0f;
    btScalar bodyMass = 40.0f;
    
    btVector3 halfExtents(0.5,0.5,0.5);
    btBoxShape* shape = new btBoxShape(halfExtents);

    btVector3 inertia;
    shape->calculateLocalInertia(bodyMass, inertia);
    
    #ifdef COLLISION_DOMAIN_SERVER
        mRigidBody = new btRigidBody( bodyMass, NULL, shape, inertia );
    #else
        mRigidBody = new btRigidBody( bodyMass, (btMotionState *)( new BtOgre::RigidBodyState( mNode ) ), shape, inertia );
    #endif
    
    mRigidBody->setRestitution( bodyRestitution );
    mRigidBody->setFriction( bodyFriction );
    
    // We must set NO CONTACT COLLISIONS to allow cars to drive through the powerups
    mRigidBody->setUserPointer( this );
    mRigidBody->setActivationState( DISABLE_DEACTIVATION );
    //mRigidBody->translate( BtOgre::Convert::toBullet( spawnAt ) );
    mRigidBody->setWorldTransform( btTransform( btQuaternion::btQuaternion().getIdentity(), BtOgre::Convert::toBullet( spawnAt ) ) );

    #ifdef COLLISION_DOMAIN_SERVER
        GameCore::mPhysicsCore->addRigidBody( mRigidBody, COL_POWERUP, COL_CAR | COL_POWERUP | COL_ARENA );
    #else
        GameCore::mPhysicsCore->addRigidBody( mRigidBody, COL_POWERUP, COL_POWERUP | COL_ARENA );
    #endif
}

Powerup::~Powerup()
{
    #ifdef COLLISION_DOMAIN_CLIENT
        GameCore::mAudioCore->deleteSoundInstance(mSound);

        mNode->detachAllObjects();
        mNode->getParentSceneNode()->removeChild(mNode);

        if ( GameCore::mClientGraphics->mBigScreen != NULL )
        {
            GameCore::mClientGraphics->mBigScreen->removePowerupOverlayFromMap(mUniqueID);
        }

        // don't destroy the big screen overlay element there are some race conditions
        //Ogre::OverlayManager::getSingleton().destroyOverlayElement(mBigScreenOverlayElement);
    #endif
    
    // delete the shape last
    btCollisionShape* collisionShape = mRigidBody->getCollisionShape();
    GameCore::mPhysicsCore->removeBody( mRigidBody );
    delete collisionShape;
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

        // mBigScreenOverlayElement will be hidden when the destructor is called
    #endif
    
    // don't delete this yet as it could be in the middle of a timestep
    mRigidBody->setUserPointer(NULL);
    
    #ifdef COLLISION_DOMAIN_SERVER
        GameCore::mNetworkCore->sendPowerupCollect( mPoolIndex, player );
    #endif

    if (player != NULL)
    {
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
    }    
}

void Powerup::frameEvent(const float timeSinceLastFrame)
{
    /*if (hasFullyDroppedIn) return;
    
    #ifdef COLLISION_DOMAIN_CLIENT
        Ogre::Vector3 currentPosition = mNode->getPosition();
        float distanceToFinalPosition = SpawnAtPosition.y - currentPosition.y;
        
        distanceToFinalPosition *= 100;
        distanceToFinalPosition *= 1.0f/60.0f * timeSinceLastFrame; // framerate independent drop

        // the powerup, before being moved is within 6cm, lets just finish it
        if (currentPosition.y - distanceToFinalPosition < SpawnAtPosition.y + 0.06 
            && currentPosition.y - distanceToFinalPosition > SpawnAtPosition.y - 0.06 )
        {
            mNode->setPosition(SpawnAtPosition);
            hasFullyDroppedIn = true;
            return;
        }
        
        currentPosition.y -= abs(distanceToFinalPosition);
        mNode->setPosition(currentPosition);
    #endif*/
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
    return BtOgre::Convert::toOgre(mRigidBody->getWorldTransform().getOrigin());
}

btQuaternion Powerup::getRotation()
{
    return mRigidBody->getWorldTransform().getRotation();
}

Ogre::OverlayElement* Powerup::getBigScreenOverlayElement()
{
    return mBigScreenOverlayElement;
}

void Powerup::reinitBigScreenOverlayElementIfNull()
{
    #ifdef COLLISION_DOMAIN_CLIENT
        if ( !mBigScreenOverlayElement
            && GameCore::mClientGraphics->mBigScreen )
        {
            mBigScreenOverlayElement = GameCore::mClientGraphics->mBigScreen->createPowerupOverlayElement(getPosition(), mUniqueID);
        }
    #endif
}
