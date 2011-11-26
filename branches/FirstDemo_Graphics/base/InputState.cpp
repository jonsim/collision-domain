/**
 * @file	InputState.h
 * @brief 	Contains an uneditable snapshot of the player's input state 
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InputState.h"



/*-------------------- FUNCTION DEFINITIONS --------------------*/

/// @brief  Constructor, storing the two values provided.
/// @param  frwdBack    The state of the forward/back keys.
/// @param  leftRght    The state of the left/right keys.
InputState::InputState (PlayerKeyState frwdBack, PlayerKeyState leftRght)
{
    forwardBack = frwdBack;
    leftRight = leftRght;
}


/// @brief  Deconstructor.
InputState::~InputState (void)
{
}


/// @brief  Returns the state of the forward/back keys.
/// @return The forward/back key state.
PlayerKeyState getFrwdBack (void)
{
    return forwardBack;
}


/// @brief  Returns the state of the left/right keys.
/// @return The left/right key state.
PlayerKeyState getLeftRght (void)
{
    return leftRight;
}