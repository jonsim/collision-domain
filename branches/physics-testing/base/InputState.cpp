/**
 * @file	InputState.h
 * @brief 	An uneditable snapshot of the player's input state 
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InputState.h"



/*-------------------- FUNCTION DEFINITIONS --------------------*/
/// @brief  Constructor, setting both inputs to zero (no input).
InputState::InputState (void)
{
    forwardBack = ZERO;
    leftRight = ZERO;
}


/// @brief  Constructor, storing the two values provided.
/// @param  frwdBack    The state of the forward/back keys.
/// @param  leftRght    The state of the left/right keys.
InputState::InputState (InputState::PlayerKeyState frwdBack, InputState::PlayerKeyState leftRght)
{
    forwardBack = frwdBack;
    leftRight = leftRght;
}

/// @brief  Constructor, calculating the PlayerKeyStates from the booleans provided.
/// @param  frwdPressed  Whether the forward key is pressed.
/// @param  backPressed  Whether the back key is pressed.
/// @param  leftPressed  Whether the left key is pressed.
/// @param  rghtPressed  Whether the right key is pressed.
InputState::InputState (bool frwdPressed, bool backPressed, bool leftPressed, bool rghtPressed)
{
    forwardBack = ZERO;
    leftRight   = ZERO;
    if (frwdPressed)
        forwardBack += 1;
    if (backPressed)
        forwardBack -= 1;
    if (leftPressed)
        leftRight   += 1;
    if (rghtPressed)
        leftRight   -= 1;
}


/// @brief  Deconstructor.
InputState::~InputState (void)
{
}


/// @brief  Returns the state of the forward/back keys.
/// @return The forward/back key state.
InputState::PlayerKeyState InputState::getFrwdBack (void)
{
    return forwardBack;
}


/// @brief  Returns the state of the left/right keys.
/// @return The left/right key state.
InputState::PlayerKeyState InputState::getLeftRght (void)
{
    return leftRight;
}