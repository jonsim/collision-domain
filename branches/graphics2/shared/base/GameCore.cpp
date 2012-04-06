/**
 * @file	Entities.cpp
 * @brief 	An attempt at a neat place to keep all the stuff which is
                gradually piling up in GraphicsCore and GraphicsApplication.
 */

#include "stdafx.h"
#include "SharedIncludes.h"


Ogre::SceneManager* GameCore::mSceneMgr = NULL;
PlayerPool*  GameCore::mPlayerPool = NULL;

#ifdef COLLISION_DOMAIN_SERVER
AiCore*					GameCore::mAiCore				= NULL;
ServerGraphics*         GameCore::mServerGraphics       = NULL;
#else
GraphicsCore*			GameCore::mGraphicsCore			= NULL;
GraphicsApplication*	GameCore::mGraphicsApplication	= NULL;
#endif
NetworkCore*			GameCore::mNetworkCore			= NULL;
PhysicsCore*			GameCore::mPhysicsCore			= NULL;
AudioCore*				GameCore::mAudioCore			= NULL;
GameGUI*				GameCore::mGui					= NULL;
PowerupPool*			GameCore::mPowerupPool			= NULL;
Gameplay*				GameCore::mGameplay				= NULL;

#ifdef COLLISION_DOMAIN_CLIENT
void GameCore::initialise(GraphicsCore* graphicsCore, SplashScreen* ss, int progress)
#else
void GameCore::initialise(SplashScreen* ss, int progress)
#endif
{
    int progressStep = (float) (100 - progress) / 8.0f;

#ifdef COLLISION_DOMAIN_SERVER
    ss->updateProgressBar(progress, "Loading AI..."); // 1
	GameCore::mAiCore		= new AiCore();
#else
    ss->updateProgressBar(progress, ""); // 1
    GameCore::mGraphicsCore = graphicsCore;
#endif
    ss->updateProgressBar(progress += progressStep, "Loading GUI..."); // 1

	GameCore::mGui         = new GameGUI();
    ss->updateProgressBar(progress += progressStep, "Loading Players..."); // 2

    GameCore::mPlayerPool  = new PlayerPool();
    ss->updateProgressBar(progress += progressStep, "Loading Network..."); // 3

    GameCore::mNetworkCore = new NetworkCore();
    ss->updateProgressBar(progress += progressStep, "Loading Physics..."); // 4

    GameCore::mPhysicsCore = new PhysicsCore();
    ss->updateProgressBar(progress += progressStep, "Loading Audio..."); // 5

    GameCore::mAudioCore   = new AudioCore();
    ss->updateProgressBar(progress += progressStep, "Loading Powerups..."); // 6

    GameCore::mPowerupPool = new PowerupPool();
    ss->updateProgressBar(progress += progressStep, "Loading Gameplay..."); // 7

	GameCore::mGameplay	   = new Gameplay();
    ss->updateProgressBar(100, "Finalising...");                      // 8
}

void GameCore::destroy()
{
    // TODO: DESTROY THE OTHERS? (also sceneManager doesn't exist by the
    //       time this method is called, which could really mess up physics

    delete GameCore::mNetworkCore;
}
