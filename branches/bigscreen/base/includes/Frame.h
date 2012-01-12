/**
 * @file	Frame.h
 * @brief 	Contains the information needed to calculate a frame.
 */
#ifndef FRAME_H
#define FRAME_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InputState.h"
#include "PlayerState.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief 	Contains the information needed to calculate a frame.
 */
class Frame
{

public:
    Frame (void);
    Frame (PlayerState player, InputState input, Ogre::Real tslf);
    ~Frame (void);
    PlayerState calculateNewState (void);
    PlayerState recalculateState (PlayerState updatedState);

private:
    PlayerState playerState;	///< The player's state at the time of rendering the frame.
    InputState	inputState;		///< The player's input state at the time of rendering the frame.
    Ogre::Real	timeSinceLastFrame;	///< The time since the last frame was rendered.
};

#endif // #ifndef FRAME_H
