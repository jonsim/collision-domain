/**
 * @file	Entities.cpp
 * @brief 	An attempt at a neat place to keep all the stuff which is
                gradually piling up in GraphicsCore and GraphicsApplication.
 */

#include "stdafx.h"
#include "SharedIncludes.h"


Ogre::SceneManager* GameCore::mSceneMgr;
PlayerPool*  GameCore::mPlayerPool;

GraphicsCore* GameCore::mGraphicsCore;
NetworkCore* GameCore::mNetworkCore;
PhysicsCore* GameCore::mPhysicsCore;


void GameCore::initialise(GraphicsCore* graphicsCore, Ogre::SceneManager* sceneMgr)
{
    GameCore::mSceneMgr = sceneMgr;
    GameCore::mPlayerPool = new PlayerPool();

    GameCore::mGraphicsCore = graphicsCore;
    GameCore::mNetworkCore = new NetworkCore( "192.168.0.2", SERVER_PORT, NULL ); // "localhost" in the server
    GameCore::mPhysicsCore = new PhysicsCore(GameCore::mSceneMgr);
}

void GameCore::destroy()
{
    // TODO: DESTROY THE OTHERS? (also sceneManager doesn't exist by the
    //       time this method is called, which could really mess up physics

    GameCore::mNetworkCore->~NetworkCore();
}
