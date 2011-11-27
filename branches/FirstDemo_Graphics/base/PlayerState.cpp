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
    playerTurningConstant = PI / 1.5f;
    playerAccelerationConstant = 100;
    playerTopSpeed = 100;
    playerFrictionConstant = 0.08f;

    playerLocation = Ogre::Vector3::ZERO;
    playerSpeed = 0;
    playerAcceleration = 0;
    playerRotation = 0;
}


/// @brief  Constructor, setting all values to those provided.
/// @param  location    The player's x,y,z location.
/// @param  speed       The player's speed (units / sec).
/// @param  acceleration    The player's acceleration (units / sec^2).
/// @param  rotation    The player's rotation from the z axis (degrees).
PlayerState::PlayerState (Ogre::Vector3 location, float speed, float acceleration, float rotation)
{
    playerTurningConstant = PI / 1.5f;
    playerAccelerationConstant = 100;
    playerTopSpeed = 100;
    playerFrictionConstant = 0.08f;

    playerLocation = location;
    playerSpeed = speed;
    playerAcceleration = acceleration;
    playerRotation = rotation;
}


/// @brief  Deconstructor.
PlayerState::~PlayerState (void)
{
}


/// @brief  Returns the maximum speed of the player (units / sec).
/// @return The player's maximum speed.
int PlayerState::getTopSpeed (void)
{
    return playerTopSpeed;
}


/// @brief  Returns the amount that the player linearly accelerates by when moving forward (units / sec^2).
/// @return The player's acceleration amount.
int PlayerState::getAccelerationConstant (void)
{
    return playerAccelerationConstant;
}


/// @brief  Returns the amount that the player turns when cornering (radians / sec).
/// @return The player's turning constant.
float PlayerState::getTurningConstant (void)
{
    return playerTurningConstant;
}


/// @brief  Returns the amount of energy a player loses per second to friction (percentage).
/// @return The player's friction constant.
float PlayerState::getFrictionConstant (void)
{
    return playerFrictionConstant;
}


/// @brief  Returns the player's x,y,z location.
/// @return The player's location.
Ogre::Vector3 PlayerState::getLocation (void)
{
    return playerLocation;
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


/// @brief  Returns the player's rotation (radians).
/// @return The player's rotation.
float PlayerState::getRotation (void)
{
    return playerRotation;
}