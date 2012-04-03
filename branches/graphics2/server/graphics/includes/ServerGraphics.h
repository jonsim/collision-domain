/**
 * @file    GraphicsApplication.h
 * @brief     Adds objects to the graphics interface and contains the framelistener
 */
#ifndef SERVERGRAPHICS_H
#define SERVERGRAPHICS_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief  Manages the server's graphics (a console).
 */
class ServerGraphics : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener
{
public:
    ServerGraphics (void);
    virtual ~ServerGraphics (void);
    
    virtual void go (void);
    void shutdown (void) { mShutDown = true; }

    Ogre::Camera*       mCamera;
    Ogre::RenderWindow* mWindow;

protected:
    virtual bool initApplication (void);    // Determines flow from startup
    virtual bool configureRenderer (void);  // Displays the configuration screen - will be removed from the final program.

    virtual void createCamera (void);
    virtual void createViewports (void);
    virtual void setupResources (void);
    virtual void loadResources (void);
    virtual void setupGUI (void);
    virtual void createFrameListener (void);
    
    // Ogre::FrameListener overrides
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);
    
    // Ogre::WindowEventListener overrides.
    virtual void windowResized (Ogre::RenderWindow* rw);
    virtual void windowClosed (Ogre::RenderWindow* rw);

    // OIS Input device elements
    Input mUserInput;
    
    // GUI Elements
	CEGUI::Window* mSheet;
    CEGUI::OgreRenderer* mGUIRenderer;

    // Ogre elements
    Ogre::Root*  mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    OgreBites::SdkCameraMan* mCameraMan;     // basic camera controller
    bool mCursorWasVisible;                  // Was the cursor visible before dialog appeared
    bool mShutDown;
};

#endif // #ifndef SERVERGRAPHICS_H
