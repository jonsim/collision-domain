/*------------------------------------------------------------------------------
  File:     Player.h
  Purpose:  Manages a player object.
 ------------------------------------------------------------------------------*/
#ifndef PLAYER_H
#define PLAYER_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../../graphics/includes/stdafx.h"
#include "physicsEntity.h"



/******************** CLASS DEFINITIONS ********************/
class player : public physicsEntity
{
 
public:
    player (void);
    ~player (void);
    
private:
	static const Ogre::Real playerTurningConstant;      // The amount that the player turns when cornering (?? / sec)
    static const Ogre::Real playerAccelerationConstant; // The amount that the player linearly accelerates by when moving forward (units / sec^2)
    static const Ogre::Real playerTopSpeed;             // The maximum speed of the player (units / sec)
    static const Ogre::Real playerFrictionConstant;     // The amount of energy a player loses per second to friction (percentage)
	Ogre::Real playerSpeed;             // The player's current speed (units / sec)
	Ogre::Real playerAcceleration;      // The player's current acceleration (units / sec^2)
    Ogre::Degree playerRotation;        // The player's current rotation from the z axis (degrees).

};

#endif // #ifndef PLAYER_H
