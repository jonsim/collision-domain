/**
 * @file	InputState.h
 * @brief 	An uneditable snapshot of the player's input state 
 */
#ifndef INPUTSTATE_H
#define INPUTSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	An uneditable snapshot of the player's input state 
 */
class InputState
{
public:
    InputState (bool frwdPressed, bool backPressed, bool leftPressed, bool rghtPressed);
    ~InputState ();
    bool isForward ();
    bool isBack ();
    bool isLeft ();
    bool isRight ();

private:
    bool mForward, mBack, mLeft, mRight;
};

#endif // #ifndef INPUTSTATE_H
