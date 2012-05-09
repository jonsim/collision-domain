/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef GAMECORE_H
#define GAMECORE_H

#include "stdafx.h"
#include "PlayerPool.h"
#include "PowerupPool.h"
#include "GameGUI.h"
#include "PhysicsCore.h"

#ifdef COLLISION_DOMAIN_CLIENT
#include "ClientGraphics.h"
#include "AudioCore.h"
#else
#include "ServerGraphics.h"
#include "AiCore.h"
#endif

// needed for non-shared variables like GraphicsCore and NetworkCore

class PlayerPool;
class NetworkCore;
#ifdef COLLISION_DOMAIN_CLIENT
class AudioCore;
class ClientGraphics;
#else
class AiCore;
class ServerGraphics;
#endif
class SplashScreen;
class GameGUI;
class PowerupPool;
class Gameplay;
class MeshDeformer;

#define MESH_SCALING_CONSTANT 0.01f


/**
 *  @brief 	Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */
class GameCore
{
public:
    static Ogre::SceneManager* mSceneMgr;
    static PlayerPool*  mPlayerPool;

#ifdef COLLISION_DOMAIN_CLIENT
    static AudioCore* mAudioCore;
    static ClientGraphics* mClientGraphics;
#else
	static AiCore* mAiCore;
    static ServerGraphics* mServerGraphics;
#endif
    static NetworkCore* mNetworkCore;
    static PhysicsCore* mPhysicsCore;
	static GameGUI* mGui;
    static PowerupPool* mPowerupPool;
	static Gameplay* mGameplay;

#ifdef COLLISION_DOMAIN_CLIENT
    static void initialise (ClientGraphics* clientGraphics);
#else
    static void initialise (ServerGraphics* serverGraphics);
#endif
    static void load (SplashScreen* ss, int progress);
    static void destroy();

	static MeshDeformer meshDeformer;

    static unsigned int rseed;
private:
    
};

#endif // #ifndef GAMECORE_H
