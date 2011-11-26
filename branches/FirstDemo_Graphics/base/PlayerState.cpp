/**
 * @file	PlayerState.cpp
 * @brief 	Contains the player's data such as speed, bearing etc.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "PlayerState.h"



/*-------------------- FUNCTION DEFINITIONS --------------------*/
/// @brief  Constructor, setting all values (location, speed, acceleration and rotation) to 0.
PlayerState::PlayerState (void)
{
    playerLocation = OGAR;
}


/// @brief  Constructor, setting all values to those provided.
/// @param  location    The player's x,y,z location.
/// @param  speed       The player's speed (units / sec).
/// @param  acceleration    The player's acceleration (units / sec^2).
/// @param  rotation    The player's rotation from the z axis (degrees).
PlayerState::PlayerState (Ogre::Vector location, float speed, float acceleration, int rotation)
{
    playerLocation = location;
    playerSpeed = speed;
    playerAcceleration = acceleration;
    playerRotation = rotation;
}


/// @brief  Deconstructor.
PlayerState::~PlayerState (void)
{
}


/// @brief  Returns the player's x,y,z location.
/// @return The player's location.
Ogre::Vector PlayerState::getLocation (void)
{
    return playerLocation
}


/// @brief  Returns the player's speed (units / sec).
/// @return The player's speed.
float PlayerState::getSpeed (void)
{
    return playerSpeed;
}


/// @brief  Returns the player's acceleration (units / sec^2).
/// @return The player's acceleration.
float PlayerState::getAcceleration (void)
{
    return playerAcceleration;
}


/// @brief  Returns the player's rotation (degrees).
/// @return The player's rotation.
int PlayerState::getRotation (void)
{
    return playerRotation;
}