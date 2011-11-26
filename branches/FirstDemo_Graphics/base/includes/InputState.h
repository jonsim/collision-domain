/**
 * @file	InputState.h
 * @brief 	Contains an uneditable snapshot of the player's input state 
 */
#ifndef INPUTSTATE_H
#define INPUTSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
#define POSITIVE 1
#define ZERO     0
#define NEGATIVE -1
/// CarType represents the state of a key (essentially a 3-state boolean).
/** It is done in this way as it makes the maths much easier and more elegant than if 4 separate booleans were used (which could be encoded with 4 bits (a single unsigned char)). */
typedef signed char PlayerKeyState


class InputState
{
public:
    InputState (PlayerKeyState frwdBack, PlayerKeyState leftRght);
    ~InputState (void);
    PlayerKeyState getFrwdBack (void);
    PlayerKeyState getLeftRght (void);

private:
    PlayerKeyState forwardBack;	///< The forward/back (W/S) keystate.
    PlayerKeyState leftRight;	///< The left/right (A/D) keystate.
};

#endif // #ifndef INPUTSTATE_H
