#include "stdafx.h"
#include "Powerup.h"
#include "GameCore.h"
#include "PowerupPool.h"

PowerupPool::PowerupPool()
{
    for (int i = 0; i < MAX_POWERUPS; i++) mPowerups[i] = NULL;
}

// This is called on the client to create a powerup
// This is called on the server to create a powerup and call this method again on the client
void PowerupPool::spawnPowerup(PowerupType type, Ogre::Vector3 spawnAt, int index)
{
    if (index < 0 || index >= MAX_POWERUPS)
    {
        assert(false);
        return;
    }

    if (mPowerups[index] != NULL)
        //OutputDebugString("OOPS, Forgetting about powerup in PowerupPool::spawnPowerup");

    mPowerups[index] = new Powerup(type, spawnAt, index);

    #ifdef COLLISION_DOMAIN_SERVER
        GameCore::mNetworkCore->sendPowerupCreate(index, type, spawnAt);
    #endif
}

Powerup *PowerupPool::getPowerup( int index )
{
    if( index < 0 || index >= MAX_POWERUPS ) return NULL;
    return mPowerups[index];
}

/// @brief  Delete a powerup 
/// @param  index   Index in the powerup array of which one to be deleted
void PowerupPool::deletePowerup( int index )
{
    if( index < 0 || index >= MAX_POWERUPS ) return;

    Powerup* p = mPowerups[index];
    mPowerups[index] = NULL;

    // You have to cast to the superclass to make cpp call that destructor!
    delete p;
}

/// @brief  Process state changes for powerups and delete collected ones
void PowerupPool::frameEvent( const float timeSinceLastFrame )
{
        for( int i = 0; i < MAX_POWERUPS; i ++ )
    {
        if ( mPowerups[i] )
        {
            // This has to be done here, because the instance can't be deleted in the collision
            // callback (you can't call delete() then return to the deleted object's method!)
            if ( mPowerups[i]->isPendingDelete() )
                deletePowerup( i );
            else
                mPowerups[i]->frameEvent(timeSinceLastFrame);
        }
        else
        {
            // this will fill this null index with a powerup.
            #ifdef COLLISION_DOMAIN_SERVER
                spawnPowerup( POWERUP_HEALTH, randomPointInArena(75, 50, 2), i );
            #endif
        }
    }
}

Ogre::Vector3 PowerupPool::randomPointInArena(int arenaXRadius, int arenaZRadius, const int safeZoneFromEdge)
{
    float x;
    float y = -10.0;
    float z;

    arenaZRadius -= safeZoneFromEdge;
    arenaXRadius -= safeZoneFromEdge;

    // choose a random X
    x = (rand() % ((arenaXRadius * 2) + 1)) - arenaXRadius;

    // select a random Z, which lies within the oval and along chosen X
    // assume round arena with radius = x radius
    int rSq = arenaXRadius * arenaXRadius;

    // zBound will always be positive
    float zBound = (float) Ogre::Math::Sqrt(Ogre::Real((float) rSq - x*x));
    int zBoundInt = (int) zBound;

    // choose a random Z
    z = (rand() % ((zBoundInt * 2) + 1)) - zBoundInt;

    // the arena was modelled as round, but its oval
    z *= (float) arenaZRadius / (float) arenaXRadius;

    btVector3 rayFrom(x,10,z);
    btVector3 rayTo(x,-200, z);
    btVector3 worldNormal;
    btVector3 worldHitPoint;

    if ( GameCore::mPhysicsCore->singleObjectRaytest(rayFrom, rayTo, worldNormal, worldHitPoint) )
    {
            std::stringstream overlayNameSS;
            overlayNameSS << "x" << worldHitPoint.x() << "   y" << worldHitPoint.y() << "   z" << worldHitPoint.z() << "\n";

        //OutputDebugString(overlayNameSS.str().c_str());
    }
    else
    {
        //OutputDebugString("No Hit\n");
    }


    return Ogre::Vector3(x, worldHitPoint.y() + 0.5, z);
}

// THIS WILL RETURN 0,0,0 IF THERE ARE NO POWERUPS
Ogre::Vector3 PowerupPool::getNearestPowerUp(Ogre::Vector3 pos)
{
        float minDist = std::numeric_limits<float>::infinity();
    float distance;

        Ogre::Vector3 ret(0,0,0);

        for(int i = 0; i < MAX_POWERUPS; i++)
        {
                if(mPowerups[i])
                        distance = mPowerups[i]->getPosition().distance(pos);
                else
                        distance = std::numeric_limits<float>::infinity();

                if(distance < minDist)
                {
                        minDist = distance;
                        ret = mPowerups[i]->getPosition();
                }
        }

        return ret;
}
