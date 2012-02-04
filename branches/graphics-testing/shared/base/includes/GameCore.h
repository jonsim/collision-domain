/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef GAMECORE_H
#define GAMECORE_H

#include "stdafx.h"

// needed for non-shared variables like GraphicsCore and NetworkCore
#include "GameIncludes.h"
class PlayerPool;
class NetworkCore;
class GraphicsCore;
class GraphicsApplication;
class AudioCore;
class GameGUI;
class PowerupPool;

#define MESH_SCALING_CONSTANT 0.02f


/**
 *  @brief 	Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */
class GameCore
{
public:
    static Ogre::SceneManager* mSceneMgr;
    static PlayerPool*  mPlayerPool;

    static GraphicsCore* mGraphicsCore;
	static GraphicsApplication* mGraphicsApplication;
    static NetworkCore* mNetworkCore;
    static PhysicsCore* mPhysicsCore;
    static AudioCore* mAudioCore;
	static GameGUI* mGui;
    static PowerupPool* mPowerupPool;

    static void initialise(GraphicsCore* graphicsCore, Ogre::SceneManager* sceneMgr);
    static void destroy();

private:
    
};

#endif // #ifndef GAMECORE_H
