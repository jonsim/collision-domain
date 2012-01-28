/**
 * @file	stdafx.h
 * @brief 	Centralizes headers and allows precompilation.
 *          See http://www.ogre3d.org/tikiwiki/Precompiled+headers for a tutorial on how to set this up.
 */

/*-------------------- INCLUDES --------------------*/

#include <string>

// Standard includes (it is unlikely these will need changing)
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

// OIS includes (the OIS libraries handle I/O)
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

// Additional includes
#include <SdkTrays.h>
#include <SdkCameraMan.h>

// CEGUI includes
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

//#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamics.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "Shapes/OgreBulletCollisionsCompoundShape.h"

// Windows specific include
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"
#endif
