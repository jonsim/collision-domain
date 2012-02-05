/**
 * @file	PowerupRandom.cpp
 * @brief 	Random powerup
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;

/// @brief  Constructor.
PowerupRandom::PowerupRandom(Ogre::Vector3 createAboveAt)
{
    mHasBeenCollected = false;

    int uniqueID = GameCore::mPhysicsCore->getUniqueEntityID();
    mNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode(
            "RandomPowerupNode" + boost::lexical_cast<std::string>(uniqueID));

    {
        entity = GameCore::mSceneMgr->createEntity("RandomPowerupMesh" + boost::lexical_cast<std::string>(uniqueID) , "powerup_random.mesh");
        entity->setMaterialName("powerup_random_uv");

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
        CompoundCollisionShape *compoundShape = new OgreBulletCollisions::CompoundCollisionShape();
        compoundShape->addChildShape(
            new SphereCollisionShape(Ogre::Real(1.0)),
            Ogre::Vector3(0, 0.90, 0));
        
        mRigidBody = new RigidBody(
                "RandomPowerup" + boost::lexical_cast<std::string>(uniqueID),
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
        mRigidBody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mRigidBody->disableDeactivation();
        mRigidBody->showDebugShape(false);
    }
    
    mNode->translate(createAboveAt);
}


/// @brief  Deconstructor.
PowerupRandom::~PowerupRandom()
{
    mNode->getParentSceneNode()->removeChild(mNode);
    
    // delete the shape last
    CollisionShape* collisionShape = mRigidBody->getShape();

    delete mRigidBody;
    delete collisionShape;
}


/// called when this powerup collides with a player
void PowerupRandom::playerCollision(Player* player)
{
    // this collision callback could potentially be called multiple times before
    // the collision object is removed, so give it to the first person who grabbed it
    if (mHasBeenCollected) return;
    mHasBeenCollected = true;

    Powerup* powerup;
    {
        // only a few of the powerup types fit as "random" rewards so here they are:
        PowerupType mPotentialPowerups[3] = {POWERUP_HEALTH, POWERUP_MASS, POWERUP_SPEED};

        PowerupType chosen = mPotentialPowerups[
            rand() % (sizeof(mPotentialPowerups) / sizeof(PowerupType))];
        powerup = GameCore::mPowerupPool->createPowerup(chosen);
    }

    if (powerup) powerup->playerCollision(player);

    removeFromWorlds();
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
    mRigidBody->getBulletRigidBody()->setUserPointer(NULL);
    GameCore::mPhysicsCore->mWorld->removeObject(mRigidBody);
    mNode->detachObject(entity);
}
