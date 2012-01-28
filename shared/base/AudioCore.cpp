/**
 * @file	AudioCore.cpp
 * @brief 	An interface for firing off sounds
 */

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreOggSound;

AudioCore::AudioCore()
{
    mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    mInitOK = mSoundManager->init();
    //OgreOggSound::OgreOggISound* sound = GameCore::mAudioCore->mSoundManager->createSound(name, file, stream=false, loop=false, preBuffer=false, scenemgr, immediate=false);

    // sounds which need to load immediately
    mEngineIdleSound = mSoundManager->createSound("engine-idle-1", "engine-idle-1.wav", false, true, true, GameCore::mSceneMgr, true);
    mCarHornLow = mCarCrashSound;//= mSoundManager->createSound("car-horn-low", "car-horn-low.wav", false, false, true, GameCore::mSceneMgr, false);
    mCarHornMid = mSoundManager->createSound("car-horn-mid", "car-horn-mid.wav", false, false, true, GameCore::mSceneMgr, true);
    mCarHornHigh = mCarCrashSound; //= mSoundManager->createSound("car-horn-high", "car-horn-high.wav", false, false, true, GameCore::mSceneMgr, false);

    // sounds which don't need to load immediately (background thread them)
    mCarCrashSound = mSoundManager->createSound("car-crash-1", "car-crash-1.wav", false, false, true, GameCore::mSceneMgr, false);
    mPowerupHealth = mSoundManager->createSound("powerup-health", "powerup-health.wav", false, false, true, GameCore::mSceneMgr, false);
}

/// @brief  Deconstructor.
AudioCore::~AudioCore()
{

}

/// this will end up looping the engine sounds and coordinating sounds to match rpm/speed
/// eventually the sound will be attached to the car for doppler / stereo effects
void AudioCore::playEngineIdle()
{
    if (!mEngineIdleSound->isPlaying())
    {
        mEngineIdleSound->play();
    }
}

/// will just play a simple crash sound, eventually at the location of a crash
void AudioCore::playCarCrash()
{
    if (!mCarCrashSound->isPlaying())
    {
        mCarCrashSound->play();
    }
}

/// plays a car horn (I didn't commit all the horn sounds so they will be a surprise ;)
/// eventually this will be attached to cars and networked
void AudioCore::playCarHorn()
{
    int horn = 1;

    OgreOggISound *hornSound =
        (horn == 0 ? mCarHornLow :
        (horn == 1 ? mCarHornMid :
        (horn == 2 ? mCarHornHigh : 0)));
    if (!hornSound) return;

    if (hornSound->isPlaying()) hornSound->setPlayPosition(0);
    else hornSound->play();
}

/// Will create a new sound because powerups can only be picked up once.
/// Eventually will be networked so players recieve a "ping" when a new powerup spawns
/// and so that they can hear other players picking up powerups (positional sounds)
void AudioCore::playHealthPowerup()
{
    // will need to declare one sound object for this per powerup (so we can hear them simultaneously!)
    if (!mPowerupHealth->isPlaying()) mPowerupHealth->play();
    else
    {
        mPowerupHealth->setPlayPosition(0);
        mPowerupHealth->play();
    }
}