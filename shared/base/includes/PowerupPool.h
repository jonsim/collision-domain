/**
 * @file	PowerupPool.h
 * @brief 	Handles creation, management and deletion of powerups
 */
#ifndef POWERUPPOOL_H
#define POWERUPPOOL_H

#include "stdafx.h"

#include "Powerup.h"

#define MAX_POWERUPS 4

class PowerupPool
{
public:
    PowerupPool();
    ~PowerupPool();

    void frameEvent( const float timeSinceLastFrame );
    void spawnPowerup(PowerupType type, Ogre::Vector3 spawnAt, int index);
    void replaceCurrentPowerups();

    Ogre::Vector3 getNearestPowerUp(Ogre::Vector3 pos);
    Powerup *getPowerup( int id );
    std::vector<Powerup *> getPowerups();
    
private:
    void deletePowerup( int index );
    Ogre::Vector3 randomPointInArena(int arenaXRadius, int arenaZRadius, const int safeZoneFromEdge, float y);

    Powerup *mPowerups[MAX_POWERUPS];
    float secondsTilNextSpawn;
};

#endif // #ifndef POWERUPPOOL_H
