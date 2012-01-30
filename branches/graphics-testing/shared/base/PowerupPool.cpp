
#include "stdafx.h"
#include "SharedIncludes.h"

PowerupPool::PowerupPool()
{
    for( int i = 0; i < MAX_POWERUPS; i ++ )
        mPowerups[i] = NULL;

    // Random seed
    srand( (unsigned int) RakNet::GetTimeMS() );
}

Powerup *PowerupPool::createPowerup( int iType )
{
    int i = 0, iNew = -1;
	for( i = 0; i < MAX_POWERUPS; i ++ )
	{
		if( mPowerups[i] == NULL )
		{
			iNew = i;
			break;
		}
	}

	if( iNew != -1 )
	{
        // There's probably a more efficient way to do this
        // Create an instance of the relevant powerup
        switch( iType )
        {
        case POWERUP_HEALTH:
            mPowerups[iNew] = new PowerupHealth();
            break;
        case POWERUP_MASS:
            mPowerups[iNew] = new PowerupMass();
            break;
        case POWERUP_RANDOM:
            mPowerups[iNew] = new PowerupRandom();
            break;
        case POWERUP_SPEED:
            mPowerups[iNew] = new PowerupSpeed();
            break;
        }

        return mPowerups[iNew];
	}
    
    log( "Bad pickup type %i", iType );
    return NULL;
}

/// @brief  Delete a powerup 
/// @param  index   Index in the powerup array of which one to be deleted
void PowerupPool::deletePowerup( int index )
{
    if( index < 0 || index >= MAX_POWERUPS )
        return;

    // Call whatever methods in the Powerup class to handle removing things from the scene

    mPowerups[index] = NULL;
}

/// @brief  Process state changes for powerups and delete collected ones
void PowerupPool::frameEvent()
{
    int i = 0, iNew = -1;
	for( i = 0; i < MAX_POWERUPS; i ++ )
    {
        if( mPowerups[i] == NULL )
            continue;

        // This has to be done here, because the instance can't be deleted in the collision  
        // callback (you can't call delete() then return to the deleted object's method!)
        if( mPowerups[i]->mHasBeenCollected )
            deletePowerup( i );
        else
        {
            // do some nice rotation
        }
    }
}