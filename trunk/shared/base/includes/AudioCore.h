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
enum SoundType
{
    HORN_LOW,
    HORN_MID,
    HORN_HIGH,
    CAR_CRASH
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
    OgreOggISound*  getSoundInstance(SoundType h,    int uniqueID);
    OgreOggISound*  getSoundInstance(PowerupType p, int uniqueID);
    void            deleteSoundInstance(OgreOggISound* sound);
    void            frameEvent(float rpm);

private:
    void processSoundDeletesPending();

    OgreOggSoundManager* mSoundManager;
    std::list<OgreOggISound*> *mSoundDeletesPending;
    bool mInitOK;

    OgreOggISound *mEngineLow, *mEngineHigh;
};

#endif // #ifndef AUDIOCORE_H
