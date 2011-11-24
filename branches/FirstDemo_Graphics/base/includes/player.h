/*------------------------------------------------------------------------------
  File:     Player.h
  Purpose:  Manages a player object.
 ------------------------------------------------------------------------------*/
#ifndef PLAYER_H
#define PLAYER_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../../graphics/includes/stdafx.h"
#include "../../base/includes/playerSnapshot.h"
#include "physicsEntity.h"



enum CarType {SMALL, MEDIUM, LARGE};
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

/******************** CLASS DEFINITIONS ********************/
class player : public physicsEntity
{
 
public:
    player (void);
    ~player (void);
    void createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s);
    void attachCamera (Ogre::Camera* cam);
    void calculateState (playerSnapshot snapshot, Ogre::Real timeSinceLastFrame);
    
private:
	Ogre::Real playerTurningConstant;      // The amount that the player turns when cornering (?? / sec)
    Ogre::Real playerAccelerationConstant; // The amount that the player linearly accelerates by when moving forward (units / sec^2)
    Ogre::Real playerTopSpeed;             // The maximum speed of the player (units / sec)
    Ogre::Real playerFrictionConstant;     // The amount of energy a player loses per second to friction (percentage)

	Ogre::Real playerSpeed;             // The player's current speed (units / sec)
	Ogre::Real playerAcceleration;      // The player's current acceleration (units / sec^2)
    Ogre::Degree playerRotation;        // The player's current rotation from the z axis (degrees).

    Ogre::SceneNode *playerNode;
    Ogre::SceneNode *carNode;
    Ogre::SceneNode *camNode;
};

#endif // #ifndef PLAYER_H
