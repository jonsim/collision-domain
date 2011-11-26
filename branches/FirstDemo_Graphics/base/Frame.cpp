/**
 * @file	Frame.cpp
 * @brief 	Contains the information needed to calculate a frame.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Frame.h"



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
}


/// @brief  Recalculates the state of the next frame based on the updated PlayerState provided and the InputState captured in this frame.
/// @param  updatedState    The new PlayerState to use when recalculating the subsequent PlayerState.
/// @return The new PlayerState which should be rendered in the next frame.
PlayerState Frame::recalculateState (PlayerState updatedState)
{
}


/*void player::calculateState (playerSnapshot snapshot, Ogre::Real timeSinceLastFrame)
{
    playerSpeed += ((int) snapshot.revealForward()) * playerAccelerationConstant * timeSinceLastFrame;
    playerSpeed -= playerSpeed * playerFrictionConstant * timeSinceLastFrame;
    if (playerSpeed > playerTopSpeed)
        playerSpeed = playerTopSpeed;
    
    playerRotation = ((int) snapshot.revealTurn()) * playerTurningConstant;

    std::cerr << "Speed = " << playerSpeed << std::endl;

    playerNode->translate(Ogre::Vector3(0, 0, playerSpeed * timeSinceLastFrame), Ogre::Node::TS_LOCAL);
    playerNode->yaw(Ogre::Degree(playerRotation), Ogre::Node::TS_WORLD);
}*/