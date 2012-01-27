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
    mCarHornLow = mCarCrashSound;//= mSoundManager->createSound("car-horn-low", "car-horn-low.wav", false, true, true, GameCore::mSceneMgr, false);
    mCarHornMid = mCarCrashSound; //= mSoundManager->createSound("car-horn-mid", "car-horn-mid.wav", false, true, true, GameCore::mSceneMgr, false);
    mCarHornHigh = mCarCrashSound; //= mSoundManager->createSound("car-horn-high", "car-horn-high.wav", false, true, true, GameCore::mSceneMgr, false);
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

void AudioCore::togglePlayCarHorn()
{
    return;
    int horn = 0;

    OgreOggISound *hornSound =
        (horn == 0 ? mCarHornLow :
        (horn == 1 ? mCarHornMid :
        (horn == 2 ? mCarHornHigh : 0)));
    if (!hornSound) return;

    if (hornSound->isPlaying()) hornSound->stop();
    else
    {
        hornSound->setPlayPosition(0);
        hornSound->play();
    }
}