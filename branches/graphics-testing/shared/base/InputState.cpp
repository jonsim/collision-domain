/**
 * @file	InputState.cpp
 * @brief 	An uneditable snapshot of the player's input state 
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"


/*-------------------- FUNCTION DEFINITIONS --------------------*/

/// @brief  Constructor, calculating the PlayerKeyStates from the booleans provided.
/// @param  frwdPressed  Whether the forward key is pressed.
/// @param  backPressed  Whether the back key is pressed.
/// @param  leftPressed  Whether the left key is pressed.
/// @param  rghtPressed  Whether the right key is pressed.
InputState::InputState (bool frwdPressed, bool backPressed, bool leftPressed, bool rghtPressed, bool hndbPressed )
{
    mForward = frwdPressed;
    mBack = backPressed;
    mLeft = leftPressed;
    mRight = rghtPressed;
    mHandbrake = hndbPressed;
}


/// @brief  Deconstructor.
InputState::~InputState ()
{
}


/// @brief  Returns the state of the forward/back keys.
/// @return The forward key state.
bool InputState::isForward ()
{
    return mForward;
}


/// @brief  Returns the state of the forward/back keys.
/// @return The forward key state.
bool InputState::isBack ()
{
    return mBack;
}


/// @brief  Returns the state of the left/right keys.
/// @return The left/right key state.
bool InputState::isLeft ()
{
    return mLeft;
}


/// @brief  Returns the state of the left/right keys.
/// @return The left/right key state.
bool InputState::isRight ()
{
    return mRight;
}

/// @brief  Returns the state of the handbrake key.
/// @return The handbrake key state.
bool InputState::isHandbrake ()
{
    return mHandbrake;
}
