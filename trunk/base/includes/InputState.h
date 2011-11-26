/**
 * @file	InputState.h
 * @brief 	Contains an uneditable snapshot of the player's input state 
 */
#ifndef INPUTSTATE_H
#define INPUTSTATE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
class InputState
{
/// PlayerKeyState represents the state of a key (essentially a 3-state boolean).
/** It is done in this way as it makes the maths much easier and more elegant than if 4 separate booleans were used
 *  (which could be encoded with 4 bits (a single unsigned char)). It also uses less space than if an enum were used
 *  as enums always used signed 32-bit ints to represent their values. */   
typedef signed char PlayerKeyState;
#define POSITIVE 1
#define ZERO     0
#define NEGATIVE -1

public:
    InputState (void);
    InputState (PlayerKeyState frwdBack, PlayerKeyState leftRght);
    InputState (bool frwdPressed, bool backPressed, bool leftPressed, bool rghtPressed);
    ~InputState (void);
    PlayerKeyState getFrwdBack (void);
    PlayerKeyState getLeftRght (void);

private:
    PlayerKeyState forwardBack;	///< The forward/back (W/S) keystate.
    PlayerKeyState leftRight;	///< The left/right (A/D) keystate.
};

#endif // #ifndef INPUTSTATE_H
