/**
 * @file	Entities.cpp
 * @brief 	An attempt at a neat place to keep all the stuff which is
                gradually piling up in GraphicsCore and GraphicsApplication.
 */

#include "stdafx.h"
#include "SharedIncludes.h"


Ogre::SceneManager* GameCore::mSceneMgr = NULL;
PlayerPool*  GameCore::mPlayerPool = NULL;

GraphicsCore* GameCore::mGraphicsCore = NULL;
GraphicsApplication* GameCore::mGraphicsApplication = NULL;
NetworkCore*  GameCore::mNetworkCore = NULL;
PhysicsCore*  GameCore::mPhysicsCore = NULL;
AudioCore*    GameCore::mAudioCore = NULL;
GameGUI*      GameCore::mGui = NULL;
PowerupPool*  GameCore::mPowerupPool = NULL;


void GameCore::initialise(GraphicsCore* graphicsCore, Ogre::SceneManager* sceneMgr)
{
    GameCore::mSceneMgr = sceneMgr;
    GameCore::mPlayerPool = new PlayerPool();

    GameCore::mGraphicsCore = graphicsCore;
    GameCore::mNetworkCore = new NetworkCore();
    GameCore::mPhysicsCore = new PhysicsCore(GameCore::mSceneMgr);
    GameCore::mAudioCore = new AudioCore();
	GameCore::mGui = new GameGUI();
    GameCore::mPowerupPool = new PowerupPool();
}

void GameCore::destroy()
{
    // TODO: DESTROY THE OTHERS? (also sceneManager doesn't exist by the
    //       time this method is called, which could really mess up physics

    GameCore::mNetworkCore->~NetworkCore();
}
