/**
 * @file	PlayerState.h
 * @brief 	Contains the player's data such as speed, bearing etc.
 */
#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
class PlayerState
{
public:
	PlayerState (void);
    PlayerState (Ogre::Vector location, float speed, float acceleration, int rotation);
    ~PlayerState (void);
    Ogre::Vector getLocation (void);
    float getSpeed (void);
	float getAcceleration (void);
	int   getRotation (void);

private:
	Ogre::Vector playerLocation;	///< The player's current x,y,z location.
	float playerSpeed;             	///< The player's current speed (units / sec).
	float playerAcceleration;      	///< The player's current acceleration (units / sec^2).
    int   playerRotation;        	///< The player's current rotation from the z axis (degrees).
};

#endif // #ifndef PLAYERSTATE_H
