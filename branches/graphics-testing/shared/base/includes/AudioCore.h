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

//class Powerup;
//class PowerupRandom;

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
    void playCarCrash();
    void playCarHorn();
    void playHealthPowerup();
    void playSpeedPowerup();

private:
    OgreOggSoundManager* mSoundManager;
    OgreOggISound *mEngineIdleSound, *mCarCrashSound, *mCarHornLow, *mCarHornMid, *mCarHornHigh, *mPowerupHealth, *mPowerupSpeed;
    bool mInitOK;
};

#endif // #ifndef AUDIOCORE_H
