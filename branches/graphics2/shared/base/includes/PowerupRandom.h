/**
 * @file	PowerupRandom.h
 * @brief 	Random powerup.
 */
#ifndef POWERUPRANDOM_H
#define POWERUPRANDOM_H

#include "stdafx.h"
#include "SharedIncludes.h"

class Player;
class GameCore;
enum PowerupType;

/**
 *  @brief 	Random powerup
 */
class PowerupRandom : public Powerup
{
public:
    PowerupRandom(Ogre::Vector3 createAboveAt);
    ~PowerupRandom();
    void playerCollision(Player* player);
    void playerCollision(Player* player, PowerupType pwrType);
    void frameEvent( const float timeSinceLastFrame );
    bool isPendingDelete();

private:
    void removeFromWorlds();
};

#endif // #ifndef POWERUPRANDOM_H
