/**
 * @file	InputState.h
 * @brief 	An interface for firing off sounds
 */
#ifndef AUDIOCORE_H
#define AUDIOCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "OgreOggSound.h"
#include "Powerup.h"

using namespace OgreOggSound;

enum SoundType
{
    HORN_LOW,
    HORN_MID,
    HORN_HIGH,
    ENGINE_SMALL,
    ENGINE_COUPE,
    ENGINE_TRUCK,
    GEAR_CHANGE,
    CAR_CRASH,
    EXPLOSION
};

#define ENGINE_MAX_VOLUME 0.55f

/**
 *  @brief 	An uneditable snapshot of the player's input state
 */
class AudioCore
{
public:
    AudioCore();
    ~AudioCore();
    void            playSoundOrRestart(OgreOggISound *sound);
    OgreOggISound*  getSoundInstance(SoundType h,   int uniqueID, Ogre::SceneNode *attachTo, bool loop = false);
    OgreOggISound*  getSoundInstance(PowerupType p, int uniqueID, Ogre::SceneNode *attachTo);
    void            deleteSoundInstance(OgreOggISound* sound);
    void            frameEvent(Ogre::Real timeSinceLastFrame);
    void            menuToRockTrack(bool reverse);
    
private:
    void processSoundDeletesPending();
    OgreOggISound* getNamedSoundInstance(std::string name, std::string file, Ogre::SceneNode *attachTo, bool loop = false);

    OgreOggSoundManager* mSoundManager;
    std::list<OgreOggISound*> *mSoundDeletesPending;
    bool mInitOK;

    OgreOggISound *mBackingTrack, *mMenuTrack;
};

#endif // #ifndef AUDIOCORE_H
