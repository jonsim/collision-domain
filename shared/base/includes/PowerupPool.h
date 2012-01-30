/**
 * @file	PowerupPool.h
 * @brief 	Handles creation, management and deletion of powerups
 */
#ifndef POWERUPPOOL_H
#define POWERUPPOOL_H

#include "stdafx.h"
#include "SharedIncludes.h"

#include "Powerup.h"
#include "PowerupSpeed.h"
#include "PowerupMass.h"
#include "PowerupHealth.h"
#include "PowerupRandom.h"

#define MAX_POWERUPS 20

enum
{
    POWERUP_HEALTH,
    POWERUP_MASS,
    POWERUP_RANDOM,
    POWERUP_SPEED,

    POWERUP_COUNT, // Num of powerup types
};

class PowerupPool
{
private:
    Powerup *mPowerups[MAX_POWERUPS];

public:
    PowerupPool();
    ~PowerupPool();

    Powerup *createPowerup( int iType );
    void deletePowerup( int index );
    Powerup *getPowerup();

    void frameEvent( const Ogre::FrameEvent& evt );
};

#endif // #ifndef POWERUPPOOL_H