#include "stdafx.h"
#include "Powerup.h"
#include "GameCore.h"
#include "PowerupPool.h"

PowerupPool::PowerupPool()
    : secondsTilNextSpawn(0)
{
    for (int i = 0; i < MAX_POWERUPS; i++)
    {
        mPowerups[i] = NULL;
        mPowerupsLifetime[i] = 0.0f;
    }
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
    {
#ifdef COLLISION_DOMAIN_CLIENT
    #ifdef PARTICLE_EFFECT_SPLINTERS
        GameCore::mClientGraphics->generateSplinters(mPowerups[index]->getPosition());
    #endif
#endif

        delete mPowerups[index];
        mPowerups[index] = NULL;
        OutputDebugString("OOPS, Forgetting about powerup in PowerupPool::spawnPowerup\n");
    }

    mPowerups[index] = new Powerup(type, spawnAt, index);
    mPowerupsLifetime[index] = 0.0f;

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
    
#ifdef COLLISION_DOMAIN_CLIENT
    #ifdef PARTICLE_EFFECT_SPLINTERS
        GameCore::mClientGraphics->generateSplinters(mPowerups[index]->getPosition());
    #endif
#endif

    Powerup* p = mPowerups[index];
    mPowerups[index] = NULL;

    // You have to cast to the superclass to make cpp call that destructor, or make it virtual in base class!
    delete p;
}

void PowerupPool::replaceCurrentPowerups()
{
    #ifdef COLLISION_DOMAIN_SERVER
        for (int i = 0; i < MAX_POWERUPS; i++)
        {
            if (mPowerups[i] != NULL) mPowerups[i]->playerCollision(NULL);
        }
    #endif
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
            if ( mPowerups[i]->isPendingDelete() || mPowerupsLifetime[i] > 30 )
            {
                deletePowerup( i );
            }
            else
            {
                mPowerups[i]->frameEvent(timeSinceLastFrame);
                mPowerupsLifetime[i] += timeSinceLastFrame;
            }
        }
        else
        {
            // this will fill this null index with a powerup.
            #ifdef COLLISION_DOMAIN_SERVER
                if (secondsTilNextSpawn < 0)
                {
                    secondsTilNextSpawn = rand() % 4;

                    bool isQuarry = GameCore::mGameplay->getArenaID() == QUARRY_ARENA;

                    int xRad = isQuarry ? 60 : 110;
                    int yRad = isQuarry ? 60 : 73;

                    spawnPowerup( (PowerupType) ( rand() % POWERUP_COUNT ), randomPointInArena(110, 73, 2, (isQuarry ? 35.0f : 10.0f ) ), i );
                }
                else
                {
                    secondsTilNextSpawn -= timeSinceLastFrame;
                }
            #endif
        }
    }
}

Ogre::Vector3 PowerupPool::randomPointInArena(int arenaXRadius, int arenaZRadius, const int safeZoneFromEdge, float y)
{
    float x;
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

    /*btVector3 rayFrom( x, 10, z );
    btVector3 rayTo( x, -200, z );
    btVector3 worldNormal;

    // If the ray misses because of some error we can just spawn it under the floor
    btVector3 worldHitPoint( 0, -100, 0 );

    if ( ! GameCore::mPhysicsCore->singleObjectRaytest(rayFrom, rayTo, worldNormal, worldHitPoint) )
    {
        OutputDebugString("Powerup floor finder ray missed\n");
    }*/

    return Ogre::Vector3(x, y, z);
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

std::vector<Powerup *> PowerupPool::getPowerups()
{
	//Put all the players from the array into a vector
	std::vector<Powerup *> tmp;
	for (unsigned int i = 0; i < MAX_POWERUPS; i++)
	{
		if (mPowerups[i])
        {
			tmp.push_back(mPowerups[i]);
        }
	}
	
	return tmp;
}
