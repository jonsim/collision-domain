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
    /*
    float newAcceleration = inputState.getFrwdBack() * playerState.getAccelerationConstant() * timeSinceLastFrame;
    float newSpeed        = playerState.getSpeed() + newAcceleration;
    if (newSpeed > playerState.getTopSpeed())
        newSpeed = playerState.getTopSpeed();*/
    //int newRotation     = playerState.getRotation() + (inputState.getLeftRght() * playerState.getTurningConstant() * timeSinceLastFrame);
    //float newRotationRad = ((float) newRotation) * (PI / 180);
    int newRotation     = inputState.getLeftRght() * playerState.getTurningConstant() * timeSinceLastFrame;
    //float newRotationRad = ((float) newRotation) * (PI / 180);
    
    float newAcceleration = 0;
    float newSpeed = playerState.getTopSpeed() * inputState.getFrwdBack() * timeSinceLastFrame;

    //Ogre::Vector3 loc = playerState.getLocation();
    //float newX = loc.x + (newSpeed * sin(newRotationRad));
    //float newZ = loc.z + (newSpeed * cos(newRotationRad));
    //float newY = loc.y;
    //Ogre::Vector3 newLocation(newX, newY, newZ);

    Ogre::Vector3 distance(0, 0, -newSpeed);

    PlayerState result(distance, newSpeed, newAcceleration, newRotation);
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