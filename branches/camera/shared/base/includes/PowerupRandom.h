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
    void frameEvent( const Ogre::FrameEvent& evt );
    bool isPendingDelete();

private:
    Ogre::Entity *entity;
    void removeFromWorlds();
};

#endif // #ifndef POWERUPRANDOM_H
