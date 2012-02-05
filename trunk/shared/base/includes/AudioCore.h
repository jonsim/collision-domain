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

enum HornType
{
    HORN_LOW,
    HORN_MID,
    HORN_HIGH
};

/**
 *  @brief 	An uneditable snapshot of the player's input state 
 */
class AudioCore
{
public:
    AudioCore();
    ~AudioCore();
    void playEngineIdle();
    void playCarCrash();
    void playCarHorn(HornType h);
    void playHealthPowerup();
    void playSpeedPowerup();

private:
    OgreOggSoundManager* mSoundManager;
    OgreOggISound *mEngineIdleSound, *mCarCrashSound, *mCarHornLow, *mCarHornMid, *mCarHornHigh, *mPowerupHealth, *mPowerupSpeed;
    bool mInitOK;
};

#endif // #ifndef AUDIOCORE_H
