/**
 * @file	PlayerState.h
 * @brief 	Contains the player's data such as speed, bearing etc.
 */
#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
#define PI (float) 3.14159265

/**
 *  @brief 	Contains the player's data such as speed, bearing etc.
 */
class PlayerState
{
public:
    PlayerState (void);
    PlayerState (Ogre::Vector3 location, float speed, float acceleration, float rotation);
    ~PlayerState (void);
    int   getTopSpeed (void);
    int   getAccelerationConstant (void);
    float getTurningConstant (void);
    float getFrictionConstant (void);
    Ogre::Vector3 getLocation (void);
    float getSpeed (void);
    float getAcceleration (void);
    float getRotation (void);

private:
    int   playerTopSpeed;             	///< The maximum speed of the player (units / sec).
    int   playerAccelerationConstant; 	///< The amount that the player linearly accelerates by when moving forward (units / sec^2).
    float playerTurningConstant;      	///< The amount that the player turns when cornering (radians / sec).
    float playerFrictionConstant;   	///< The amount of energy a player loses per second to friction (percentage).

    Ogre::Vector3 playerLocation;	    ///< The player's current x,y,z location.
    float playerSpeed;             	    ///< The player's current speed (units / sec).
    float playerAcceleration;      	    ///< The player's current acceleration (units / sec^2).
    float playerRotation;        	    ///< The player's current rotation from the z axis (radians).
};

#endif // #ifndef PLAYERSTATE_H
