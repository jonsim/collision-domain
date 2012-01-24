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
}


/// @brief  Deconstructor.
AudioCore::~AudioCore()
{

}


void AudioCore::playEngineIdle()
{
    //OgreOggSound::OgreOggISound* sound = GameCore::mAudioCore->mSoundManager->createSound(name, file, stream=false, loop=false, preBuffer=false, scenemgr, immediate=false);
    OgreOggISound* sound = mSoundManager->createSound("engine-idle-1" + GameCore::mSceneMgr->createSceneNode()->getName(), "engine-idle-1.wav", false, true, true, GameCore::mSceneMgr, false);
    sound->play();
}
