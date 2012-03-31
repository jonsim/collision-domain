/**
 * @file    AudioCore.cpp
 * @brief     An interface for firing off sounds
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreOggSound;

#define FILE_HORN_LOW          "car-horn-low.wav"
#define FILE_HORN_MID          "car-horn-mid.wav"
#define FILE_HORN_HIGH         "car-horn-high.wav"

#define FILE_1_ENGINE_IDLE     "engine-idle-1.wav"
#define FILE_CAR_CRASH         "car-crash-1.wav"
#define FILE_EXPLOSION         "explosion.wav"

#define FILE_TRUCK_ENGINE_LOW  "truck-engine-low.ogg"
#define FILE_TRUCK_ENGINE_HIGH "truck-engine-high.ogg"

#define FILE_POWERUP_HEALTH    "powerup-health.wav"
#define FILE_POWERUP_SPEED     "powerup-speed.wav"
#define FILE_POWERUP_HEAVY     "powerup-heavy.wav"
#define FILE_POWERUP_RANDOM    "powerup-random.wav"

#define FILE_BACKING_TRACK     "rockTrack.ogg"

AudioCore::AudioCore()
{
    mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    mSoundDeletesPending = new std::list<OgreOggISound*>;
    
    mInitOK = mSoundManager->init();

    // force the soundManager to buffer immediate sounds
    if (mInitOK)
    {
        std::string tempName = "hello";
        OgreOggISound* preload;
        for (int i=0; i < 10; i++)
        {
            //                                               name      file                 stream loop   preBuffer scenemgr         immediate

            if (i == 0) preload = mSoundManager->createSound(tempName, FILE_POWERUP_HEAVY,  false, false, true, GameCore::mSceneMgr, true);
            if (i == 1) preload = mSoundManager->createSound(tempName, FILE_POWERUP_HEALTH, false, false, true, GameCore::mSceneMgr, true);
            if (i == 2) preload = mSoundManager->createSound(tempName, FILE_POWERUP_SPEED,  false, false, true, GameCore::mSceneMgr, true);
            if (i == 3) preload = mSoundManager->createSound(tempName, FILE_POWERUP_RANDOM, false, false, true, GameCore::mSceneMgr, true);

            if (i == 4) preload = mSoundManager->createSound(tempName, FILE_HORN_LOW,       false, false, true, GameCore::mSceneMgr, true);
            if (i == 5) preload = mSoundManager->createSound(tempName, FILE_HORN_MID,       false, false, true, GameCore::mSceneMgr, true);
            if (i == 6) preload = mSoundManager->createSound(tempName, FILE_HORN_HIGH,      false, false, true, GameCore::mSceneMgr, true);

            if (i == 7) preload = mSoundManager->createSound(tempName, FILE_1_ENGINE_IDLE,  false, true,  true, GameCore::mSceneMgr, true);
            if (i == 8) preload = mSoundManager->createSound(tempName, FILE_CAR_CRASH,      false, false, true, GameCore::mSceneMgr, false);
            if (i == 9) preload = mSoundManager->createSound(tempName, FILE_EXPLOSION,      false, false, true, GameCore::mSceneMgr, false);

            mSoundManager->destroySound(preload);
        }

        // init some more stuff here (don't do it unless we have openAL)
    }

    mEngineLow = mSoundManager->createSound("enginelow", FILE_TRUCK_ENGINE_LOW,  false, true, true, GameCore::mSceneMgr, false);
    mEngineHigh = mSoundManager->createSound("enginehigh", FILE_TRUCK_ENGINE_HIGH,  false, true, true, GameCore::mSceneMgr, false);

    mEngineLow->setVolume(0.5f);
    mEngineHigh->setVolume(0.6f);

    // pitch is in play rate increase (4x max) (100 = 3.976x play rate)
    mEngineHigh->setPitch(2.0f);

    mBackingTrack = mSoundManager->createSound("backingtrack", FILE_BACKING_TRACK,  false, true, true, GameCore::mSceneMgr, true);

#ifdef COLLISION_DOMAIN_CLIENT
    mEngineHigh->play();
    mBackingTrack->setVolume(0.5);
    mBackingTrack->play();
#endif

}

/// @brief  Deconstructor.
AudioCore::~AudioCore()
{
    delete mSoundDeletesPending;
}

/// plays a car horn (I didn't commit all the horn sounds so they will be a surprise ;)
/// eventually this will be attached to cars and networked
void AudioCore::playSoundOrRestart(OgreOggISound *sound)
{
    // fix for some crash, if !mInitOK
    // (example of using optical output and some app is using it for encoded (i.e. dolby))
    if (!sound) return;

    if (sound->isPlaying()) sound->stop();
    
    sound->setPlayPosition(0);
    sound->play();
}

/*OgreOggISound* AudioCore::getSoundInstance(EngineType e, int uniqueID)
{
    return mSoundManager->createSound("engine-idle-1", FILE_1_ENGINE_IDLE, false, true, true, GameCore::mSceneMgr, true);
    
    // sounds which don't need to load immediately (background thread them)
    mCarCrashSound = mSoundManager->createSound("car-crash-1", FILE_1_CRASH, false, false, true, GameCore::mSceneMgr, false);
}*/

/// it is quite possible mInitOK is false, so this may return null.
/// the ID needs to be unique to the sound type (can be same for different sounds)
OgreOggISound* AudioCore::getSoundInstance(SoundType h, int uniqueID)
{
    if (!mInitOK) return NULL;

    std::string file;

    switch (h)
    {
        case HORN_LOW:
            file = FILE_HORN_LOW;
            break;

        case HORN_MID:
            file = FILE_HORN_MID;
            break;

        case HORN_HIGH:
            file = FILE_HORN_HIGH;
            break;

        case CAR_CRASH:
            file = FILE_CAR_CRASH;
            break;

        case EXPLOSION:
            file = FILE_EXPLOSION;
            break;

        default:
            return NULL;
    }
    
    std::string name = file + boost::lexical_cast<std::string>(uniqueID);
    
    // prebuffer + immediate
    return mSoundManager->createSound(name, file,  false, false, true, GameCore::mSceneMgr, true);
}

/// it is quite possible mInitOK is false, so this may return null.
/// the ID needs to be unique to the sound type (can be same for different sounds)
OgreOggISound* AudioCore::getSoundInstance(PowerupType p, int uniqueID)
{
    if (!mInitOK) return NULL;

    std::string file;

    switch (p)
    {
        case POWERUP_HEALTH:
            file = FILE_POWERUP_HEALTH;
            break;

        case POWERUP_MASS:
            file = FILE_POWERUP_HEAVY;
            break;

        case POWERUP_RANDOM:
            file = FILE_POWERUP_RANDOM;
            break;

        case POWERUP_SPEED:
            file = FILE_POWERUP_SPEED;
            break;

        default:
            return NULL;
    }
    
    std::string name = file + boost::lexical_cast<std::string>(uniqueID);

    // prebuffer + immediate
    return mSoundManager->createSound(name, file, false, false, true, GameCore::mSceneMgr, true);
}

void AudioCore::deleteSoundInstance(OgreOggISound* sound)
{
    if (!sound) return;

    mSoundDeletesPending->insert(mSoundDeletesPending->end(), sound);
}

void AudioCore::frameEvent(float rpm)
{
    processSoundDeletesPending();

    float pitch = (rpm / 3200.0f) * 2.7;

    if (pitch < 1.0f) pitch = 1.0f;

    mEngineHigh->setPitch(pitch);
}

void AudioCore::processSoundDeletesPending()
{
    // process deletes, BUT only for sounds which have finished playing
    std::list<OgreOggISound*>::iterator i = mSoundDeletesPending->begin();

    while (i != mSoundDeletesPending->end())
    {
        OgreOggISound* sound = NULL;
        sound = *i;
        
        if (sound && !sound->isPlaying())
        {
            mSoundManager->destroySound(sound);
            mSoundDeletesPending->erase(i++); // alternatively, i = items.erase(i);
        }
        else
        {
            // leave it be for now, its still playing
            ++i;
        }
    }
}