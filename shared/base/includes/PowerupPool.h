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

enum PowerupType
{
    POWERUP_HEALTH,
    POWERUP_MASS,
    POWERUP_RANDOM,
    POWERUP_SPEED
};

class PowerupPool
{
public:
    PowerupPool();
    ~PowerupPool();
    Powerup *createPowerup( PowerupType type, int id = -1 );
    Powerup *createPowerup( PowerupType type, Ogre::Vector3 createAboveAt, int id = -1 );
    Powerup *getPowerup( int id );
    PowerupType getPowerupType( int index );
    void frameEvent( const float timeSinceLastFrame );
    void spawnSomething();
	std::vector<Powerup*> getPowerups();
    
private:
    Powerup *createPowerup( PowerupType type, Ogre::Vector3 createAboveAt, bool spawn, int id = -1 );
    void deletePowerup( int index );
    Ogre::Vector3* randomPointInArena(int arenaXRadius, int arenaZRadius, const int safeZoneFromEdge);

    Powerup *mPowerups[MAX_POWERUPS];
    PowerupType mPowerupTypes[MAX_POWERUPS];
};

#endif // #ifndef POWERUPPOOL_H