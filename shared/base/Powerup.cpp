/**
 * @file        AudioCore.cpp
 * @brief       An interface for firing off sounds
 */

#include "stdafx.h"
#include "Powerup.h"
#include "GameCore.h"
#include "TruckCar.h"

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
    mRigidBody->setUserPointer(NULL);

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
#ifdef COLLISION_DOMAIN_SERVER
void Powerup::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;
    
    // don't delete this yet as it could be in the middle of a timestep
    mRigidBody->setUserPointer(NULL);

    float extraData = 0.00;
    RakNet::TimeMS curTime = RakNet::GetTimeMS();

    if (player != NULL)
    {
        switch (mPowerupType)
        {
            case POWERUP_HEALTH:
            {

                // 50% chance of full health, 50% chance of curhealth + 50%
                extraData = ( rand() % 100 < 50 ? 0.5f : 1.f );
                if( extraData == 1.f )
                    player->setHP( INITIAL_HEALTH );
                else
                {
                    int plusHealth = player->getHP() / (float) 2;
                    if( player->getHP() + plusHealth > INITIAL_HEALTH )
                        player->setHP( INITIAL_HEALTH );
                    else
                        player->setHP( player->getHP() + plusHealth );
                }

                break;
            }

            case POWERUP_MASS:
            {
                GameCore::mGui->outputToConsole( "Player '%s' mass effect begin.\n", player->getNickname() );
                // If truck, 75% chance of getting light version
                float newmass;
                bool makeLight = (player->getCar() && dynamic_cast<TruckCar*>( player->getCar() ) && rand() % 100 < 75);
                if( makeLight )
                {
                    newmass = 0.3f;
                    extraData = 1.f;
                    curTime += 15000;
                }
                else if (rand() % 9 == 0)
                {
                    newmass = 0.3f;
                    extraData = 1.f;
                    curTime += 10000;
                }
                else
                {
                    newmass = 2.f;
                    curTime += 12000;
                }

                player->addPowerup( POWERUP_MASS, curTime );
                player->getCar()->setMass( newmass );
                player->getCar()->setEngineForce( newmass );


                break;
            }

            case POWERUP_SPEED:
            {
                float newspeed;
                // Randomized increase in speed, up to double
                extraData = rand() % 100;
                curTime += 9000;

                newspeed = (extraData + 100) / 100;

                player->addPowerup( POWERUP_SPEED, curTime );
                player->getCar()->setEngineForce( newspeed );
                break;
            }

            default:
                break;
        }

        GameCore::mNetworkCore->sendPowerupCollect( this->getIndex(), player, extraData );
    }    
}
#endif

#ifdef COLLISION_DOMAIN_CLIENT
void Powerup::playerCollision(Player* player, float extraData)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    if (player != NULL)
    {
        if ( GameCore::mPlayerPool->getLocalPlayer() == player )
            GameCore::mAudioCore->playSoundOrRestart(mSound);
           
        switch (mPowerupType)
        {
        case POWERUP_HEALTH:
            player->pushBackNewPowerupBoard(POWERUP_BOARD_HEALTH, 2.5f);
            break;

        case POWERUP_MASS:
            // trucks have a 75% chance of getting a light negative powerup instead of heavy!
            if (extraData < 1.f)
            {
                if( dynamic_cast<TruckCar*>( player->getCar() ) )
                {
                    player->pushBackNewPowerupBoard(POWERUP_BOARD_LIGHT, 15.0f);
                }
                else
                {
                    // 11% chance of getting the light negative powerup
                    player->pushBackNewPowerupBoard(POWERUP_BOARD_LIGHT, 10.0f);
                }
            }
            else
            {
                // normal heavy powerup
                player->pushBackNewPowerupBoard(POWERUP_BOARD_HEAVY, 12.0f);
            }
            break;

        case POWERUP_SPEED:
            player->pushBackNewPowerupBoard(POWERUP_BOARD_SPEED, 9.0f);
            break;

        default:
            break;
        }
    }

    // mBigScreenOverlayElement will be hidden when the destructor is called

    
    // don't delete this yet as it could be in the middle of a timestep
    mRigidBody->setUserPointer(NULL);

    RakNet::TimeMS curTime = RakNet::GetTimeMS();

    if (player != NULL)
    {
        switch (mPowerupType)
        {
            // Don't need to handle health here, as the server will send over the player's new health
            case POWERUP_MASS:
            {
            
                // If truck, 75% chance of getting light version
                float newmass;
                bool makeLight = (player->getCar() && dynamic_cast<TruckCar*>( player->getCar() ) && rand() % 100 < 75);
                if( extraData < 1.f )
                {
                    if( dynamic_cast<TruckCar*>( player->getCar() ) )
                    {
                        newmass = extraData;
                        curTime += 15000;
                    }
                    else
                    {
                        newmass = extraData;
                        curTime += 10000;
                    }
                }
                else
                {
                    newmass = extraData;
                    curTime += 12000;
                }

                player->addPowerup( POWERUP_MASS, curTime );
                player->getCar()->setMass( newmass );
                player->getCar()->setEngineForce( newmass );
                break;
            }

            case POWERUP_SPEED:
            {
                float newspeed;
                // Randomized increase in speed, up to double
                curTime += 9000;

                newspeed = (extraData + 100) / 100;

                player->addPowerup( POWERUP_SPEED, curTime );
                player->getCar()->setEngineForce( newspeed );
                break;
            }

            default:
                break;
        }
    }    
}
#endif

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
