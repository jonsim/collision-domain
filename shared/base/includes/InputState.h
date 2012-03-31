/**
 * @file    InputState.h
 * @brief     An uneditable snapshot of the player's input state 
 */
#ifndef INPUTSTATE_H
#define INPUTSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief     An uneditable snapshot of the player's input state 
 */
class InputState
{
public:
    InputState (bool frwdPressed, bool backPressed, bool leftPressed, bool rghtPressed, bool hndbPressed);
    ~InputState ();
    bool isForward ();
    bool isBack ();
    bool isLeft ();
    bool isRight ();
    bool isHandbrake ();

private:
    bool mForward, mBack, mLeft, mRight, mHandbrake;
};

#endif // #ifndef INPUTSTATE_H
