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
    CAR_CRASH,
    EXPLOSION
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
    OgreOggISound*  getSoundInstance(SoundType h,   int uniqueID, Ogre::SceneNode *attachTo);
    OgreOggISound*  getSoundInstance(PowerupType p, int uniqueID, Ogre::SceneNode *attachTo);
    void            deleteSoundInstance(OgreOggISound* sound);
    void            frameEvent(float rpm, Ogre::Real timeSinceLastFrame);
    
private:
    void processSoundDeletesPending();

    OgreOggSoundManager* mSoundManager;
    std::list<OgreOggISound*> *mSoundDeletesPending;
    bool mInitOK;

    OgreOggISound *mEngineLow, *mEngineHigh;
    OgreOggISound *mBackingTrack;
};

#endif // #ifndef AUDIOCORE_H
