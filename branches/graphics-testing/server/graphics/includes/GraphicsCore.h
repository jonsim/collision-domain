/**
 * @file	GraphicsCore.h
 * @brief 	Builds the window, loading the Ogre necessary libraries and providing 
 *          the Graphics Application with underlying functions to keep it tidy.
 *          Derived from the Ogre Tutorial Framework (BaseApplication.h).
 */
#ifndef GRAPHICSCORE_H
#define GRAPHICSCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

//class BigScreen;

/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief 	Builds the window and provides the necessary low level functions.
 *
 *          Loads the required Ogre libraries and provides the Graphics Application
 *          with underlying functions to keep it tidy.
 *          Derived from the Ogre Tutorial Framework (BaseApplication.h).
 */
class SpawnScreen;

class GraphicsCore : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener
{
public:
    GraphicsCore (void);
    virtual ~GraphicsCore (void);
    virtual void go (void);
	void shutdown() { mShutDown = true; }

    Ogre::Camera* mCamera;
    Ogre::Camera* mViewCam1;
    Ogre::Camera* mViewCam2;
    Ogre::Camera* mViewCam3;
    Ogre::Camera* mViewCam4;
    Ogre::Camera* mViewCam5;
    Ogre::Camera* mViewCam6;
    Ogre::Camera* mViewCam7;
    Ogre::Camera* mViewCam8;
    Ogre::Camera* mViewCamBlank;

    BigScreen* bigScreen;
    ViewportManager* vpm;
    Gameplay* mGameplay;

    // Had to put this in here as well even though it will never be used
    // because of Input.cpp being shared. Really annoying.
    SpawnScreen *mSpawnScreen;

protected:
    virtual bool setup(); // Moved here so network can't call :-)
    virtual bool configure(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);
    virtual void createScene(void) = 0;     // This needs to be overridden to display anything
    virtual void destroyScene(void);
    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    
    // Ogre::WindowEventListener
    // Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    // Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

    // OIS user input
    Input mUserInput;


    Ogre::Root *mRoot;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    // OgreBites
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::SdkCameraMan* mCameraMan;    // basic camera controller
    OgreBites::ParamsPanel* mDetailsPanel;  // sample details panel
    bool mCursorWasVisible;                 // was cursor visible before dialog appeared
    bool mShutDown;
	
	
};

#endif // #ifndef GRAPHICSCORE_H
