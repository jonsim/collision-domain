/**
 * @file	Frame.cpp
 * @brief 	Contains the information needed to calculate a frame.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Frame.h"
#include <math.h>



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, storing all the values given to it.
/// @param  player  The player's state at the time of rendering the frame.
/// @param  input   The player's input state at the time of rendering the frame.
/// @param  tslf    The time since the last frame was rendered.
Frame::Frame (PlayerState player, InputState input, Ogre::Real tslf)
{
    playerState = player;
    inputState  = input;
    timeSinceLastFrame = tslf;
}


/// @brief  Deconstructor.
Frame::~Frame (void)
{
}


/// @brief  Calculates a new PlayerState based on the current PlayerState and the InputState captured in this frame.
/// @return The new PlayerState which should be rendered in the next frame.
PlayerState Frame::calculateNewState (void)
{
    float newAcceleration = 0;
    
    float newRotation = playerState.getRotation() + (inputState.getLeftRght() * playerState.getTurningConstant() * timeSinceLastFrame);
    float newSpeed = playerState.getSpeed() + (inputState.getFrwdBack() * playerState.getTopSpeed() * timeSinceLastFrame);
    newSpeed = newSpeed - (newSpeed * playerState.getFrictionConstant());

    //Ogre::Vector3 translation(0, 0, newSpeed);
    //Ogre::Quaternion rotation(Ogre::Radian(newRotation), Ogre::Vector3::UNIT_Y);
    Ogre::Vector3 playerLocation = playerState.getLocation();
    float newX = playerLocation.x + (newSpeed * Ogre::Math::Sin(newRotation));
    float newZ = playerLocation.z + (newSpeed * Ogre::Math::Cos(newRotation));
    Ogre::Vector3 newLocation(newX, playerLocation.y, newZ);

    //PlayerState result(rotation * translation, newSpeed, newAcceleration, newRotation);
    PlayerState result(newLocation, newSpeed, newAcceleration, newRotation);
    return result;
}


/// @brief  Recalculates the state of the next frame based on the updated PlayerState provided and the InputState captured in this frame.
/// @param  updatedState    The new PlayerState to use when recalculating the subsequent PlayerState.
/// @return The new PlayerState which should be rendered in the next frame.
PlayerState Frame::recalculateState (PlayerState updatedState)
{
    playerState = updatedState;
    return calculateNewState();
}