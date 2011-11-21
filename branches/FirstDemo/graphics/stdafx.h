/*------------------------------------------------------------------------------
  File:     stdafx.h
  Purpose:  Centralizes headers and allows precompilation.
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
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


// Windows specific include
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"
#endif