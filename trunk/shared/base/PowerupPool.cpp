
#include "stdafx.h"
#include "SharedIncludes.h"

PowerupPool::PowerupPool()
{
    for( int i = 0; i < MAX_POWERUPS; i ++ )
        mPowerups[i] = NULL;

    // Random seed
    srand( time(NULL));//(unsigned int) RakNet::GetTimeMS() );

    createPowerup(POWERUP_SPEED);
}

/// can be used if the powerup type supports creation invisibly, if not returns null
/// returns null if no free pickup slots are available
Powerup *PowerupPool::createPowerup( PowerupType type )
{
    return createPowerup( type, Ogre::Vector3::ZERO, false );
}

/// returns null if no free pickup slots are available
Powerup *PowerupPool::createPowerup( PowerupType type, Ogre::Vector3 createAboveAt )
{
    return createPowerup( type, createAboveAt, true );
}

Powerup *PowerupPool::createPowerup( PowerupType type, Ogre::Vector3 createAboveAt, bool spawn )
{
    int iNew = -1;
	for( int i = 0; i < MAX_POWERUPS; i ++ )
	{
		if( mPowerups[i] == NULL )
		{
			iNew = i;
			break;
		}
	}

    if (iNew == -1) {
        log( "No Free pickup slots" );
        return NULL;
    }

    // Create an instance of the relevant powerup
    switch( type )
    {
    case POWERUP_HEALTH:
        mPowerups[iNew] = new PowerupHealth();
        if (spawn) ((PowerupHealth*) mPowerups[iNew])->spawn(createAboveAt);
        break;

    case POWERUP_MASS:
        //if (!spawn) return NULL; // doesn't support invisible creation
        mPowerups[iNew] = new PowerupMass();
        if (spawn) ((PowerupMass*) mPowerups[iNew])->spawn(createAboveAt);
        break;

    case POWERUP_RANDOM:
        if (!spawn) return NULL; // doesn't support invisible creation
        mPowerups[iNew] = new PowerupRandom(createAboveAt);
        break;

    case POWERUP_SPEED:
        //if (!spawn) return NULL; // doesn't support invisible creation
        mPowerups[iNew] = new PowerupSpeed();
        if (spawn) ((PowerupSpeed*) mPowerups[iNew])->spawn(createAboveAt);
        break;
    }

    mPowerupTypes[iNew] = type;

    return mPowerups[iNew];
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
    switch(mPowerupTypes[index])
    {
    case POWERUP_HEALTH: delete (PowerupHealth*) p; break;
    case POWERUP_MASS:   delete (PowerupMass*)   p; break;
    case POWERUP_RANDOM: delete (PowerupRandom*) p; break;
    case POWERUP_SPEED:  delete (PowerupSpeed*)  p; break;
    }
}

void PowerupPool::spawnSomething()
{
    // spawn a random bunch for testing!!
    PowerupType type;
    switch(rand() % 4)
    {
    case 0: type = POWERUP_MASS; break;
    case 1: type = POWERUP_HEALTH;   break;
    case 2: type = POWERUP_SPEED; break;
    case 3: type = POWERUP_RANDOM;  break;
    }
    
    createPowerup( type, *randomPointInArena(75, 50, 2) );
}

/// @brief  Process state changes for powerups and delete collected ones
void PowerupPool::frameEvent( const Ogre::FrameEvent& evt )
{
	for( int i = 0; i < MAX_POWERUPS; i ++ )
    {
        if( ! mPowerups[i] )
        {
            // this will fill this null index with a powerup.
            spawnSomething();
            continue;
        }

        // This has to be done here, because the instance can't be deleted in the collision  
        // callback (you can't call delete() then return to the deleted object's method!)
        if( mPowerups[i]->isPendingDelete() )
        {
            deletePowerup( i );
        }
        else
        {
            mPowerups[i]->frameEvent(evt);
        }
    }
}

Ogre::Vector3* PowerupPool::randomPointInArena(int arenaXRadius, int arenaZRadius, const int safeZoneFromEdge)
{
    float x;
    float y = -0.5;
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

    return new Ogre::Vector3(x, y, z);
}