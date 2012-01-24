/**
 * @file	InputState.h
 * @brief 	An interface for firing off sounds
 */
#ifndef AUDIOCORE_H
#define AUDIOCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
#include "OgreOggSound.h"

using namespace OgreOggSound;

/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	An uneditable snapshot of the player's input state 
 */
class AudioCore
{
public:
    AudioCore();
    ~AudioCore();
    void playEngineIdle();

private:
    OgreOggSoundManager* mSoundManager;
    bool mInitOK;
};

#endif // #ifndef AUDIOCORE_H
