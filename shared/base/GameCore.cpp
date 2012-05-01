/**
 * @file	Entities.cpp
 * @brief 	An attempt at a neat place to keep all the stuff which is
                gradually piling up in GraphicsCore and GraphicsApplication.
 */

#include "stdafx.h"
#include "GameCore.h"


Ogre::SceneManager* GameCore::mSceneMgr = NULL;
PlayerPool*  GameCore::mPlayerPool = NULL;

#ifdef COLLISION_DOMAIN_CLIENT
ClientGraphics*			GameCore::mClientGraphics       = NULL;
AudioCore*				GameCore::mAudioCore			= NULL;
#else
ServerGraphics*         GameCore::mServerGraphics       = NULL;
AiCore*					GameCore::mAiCore				= NULL;
#endif
NetworkCore*			GameCore::mNetworkCore			= NULL;
PhysicsCore*			GameCore::mPhysicsCore			= NULL;
GameGUI*				GameCore::mGui					= NULL;
PowerupPool*			GameCore::mPowerupPool			= NULL;
Gameplay*				GameCore::mGameplay				= NULL;

#ifdef COLLISION_DOMAIN_CLIENT
void GameCore::initialise (ClientGraphics* clientGraphics)
#else
void GameCore::initialise (ServerGraphics* serverGraphics)
#endif
{
#ifdef COLLISION_DOMAIN_SERVER
    GameCore::mServerGraphics = serverGraphics;
#else
    GameCore::mClientGraphics = clientGraphics;
#endif

    // Load the networking
    GameCore::mNetworkCore = new NetworkCore();
}

void GameCore::load (SplashScreen* ss, int progress)
{
    const int endProgress = 100;
#ifdef COLLISION_DOMAIN_CLIENT
    const int numberOfSteps = 7;
#else
    const int numberOfSteps = 7;
#endif
    const int progressStep = (float) (endProgress - progress) / (float) numberOfSteps;

#ifdef COLLISION_DOMAIN_SERVER
    ss->updateProgressBar(progress += progressStep, "Loading AI...");       // -/1
	GameCore::mAiCore = new AiCore();
#endif

    ss->updateProgressBar(progress += progressStep, "Loading GUI...");      // 1/2
	GameCore::mGui         = new GameGUI();

    ss->updateProgressBar(progress += progressStep, "Loading Players...");  // 2/3
    GameCore::mPlayerPool  = new PlayerPool();

    ss->updateProgressBar(progress += progressStep, "Loading Physics...");  // 3/4
    GameCore::mPhysicsCore = new PhysicsCore();

#ifdef COLLISION_DOMAIN_CLIENT
    ss->updateProgressBar(progress += progressStep, "Loading Audio...");    // 4/-
    GameCore::mAudioCore   = new AudioCore();
#endif

    ss->updateProgressBar(progress += progressStep, "Loading Powerups..."); // 5/5
    GameCore::mPowerupPool = new PowerupPool();

    ss->updateProgressBar(progress += progressStep, "Loading Gameplay..."); // 6/6
	GameCore::mGameplay	   = new Gameplay();

    ss->updateProgressBar(100, "Finalising...");                            // 7/7
}

void GameCore::destroy()
{
    // TODO: DESTROY THE OTHERS? (also sceneManager doesn't exist by the
    //       time this method is called, which could really mess up physics

    delete GameCore::mNetworkCore;
}
