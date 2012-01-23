/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef GAMECORE_H
#define GAMECORE_H


#include "stdafx.h"
#include "ClientIncludes.h"

/**
 *  @brief 	Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */
class GameCore
{
public:
    static Ogre::SceneManager* mSceneMgr;
    static PlayerPool*  mPlayerPool;

    static GraphicsCore* mGraphicsCore;
    static NetworkCore* mNetworkCore;
    static PhysicsCore* mPhysicsCore;

    static void initialise(GraphicsCore* graphicsCore, Ogre::SceneManager* sceneMgr);
    static void destroy();

private:
    
};

#endif // #ifndef GAMECORE_H
