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

enum PowerupType; // from PowerupPool
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
    void            playSoundOrRestart(OgreOggISound *sound);
    OgreOggISound*  getSoundInstance(HornType h,    int uniqueID);
    OgreOggISound*  getSoundInstance(PowerupType p, int uniqueID);

private:
    OgreOggSoundManager* mSoundManager;
    OgreOggISound *mEngineIdleSound, *mCarCrashSound, *mCarHornLow, *mCarHornMid, *mCarHornHigh, *mPowerupHealth, *mPowerupSpeed;
    bool mInitOK;
};

#endif // #ifndef AUDIOCORE_H
