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
    mEngineIdleSound = mSoundManager->createSound("engine-idle-1", "engine-idle-1.wav", false, true, true, GameCore::mSceneMgr, false);
    mCarCrashSound = mSoundManager->createSound("car-crash-1", "car-crash-1.wav", false, false, true, GameCore::mSceneMgr, false);
}

/// @brief  Deconstructor.
AudioCore::~AudioCore()
{

}

void AudioCore::playEngineIdle()
{
    if (!mEngineIdleSound->isPlaying())
    {
        mEngineIdleSound->play();
    }
}

void AudioCore::playCarCrash()
{
    if (!mCarCrashSound->isPlaying())
    {
        mCarCrashSound->play();
    }
}
