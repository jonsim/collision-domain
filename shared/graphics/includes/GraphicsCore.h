/**
 * @file	GraphicsCore.h
 * @brief 	Configures the graphical settings and provides the common graphical functionality.
 */
#ifndef GRAPHICSCORE_H
#define GRAPHICSCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief 	Builds the window and provides the necessary low level functions.
 */
class SpawnScreen;

enum GraphicsState
{
    LOBBY,
    PLAYER,
    SPECTATOR,
    PROJECTOR
};

class GraphicsCore : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public SceneSetup
{
public:
    GraphicsCore (void);
    virtual ~GraphicsCore (void);
    virtual void go (void);
	void shutdown() { mShutDown = true; }
    void generateExplosion (Ogre::Vector3 location);
    void generateSparks (Ogre::Vector3 location, Ogre::Vector3 direction);
    void updateVIPLocation (int teamNumber, Ogre::Vector3 location);

    Ogre::Camera*       mCamera;
    Ogre::RenderWindow* mWindow;
    SpawnScreen *mSpawnScreen;
    

protected:
    // Called when the application is first started.
    virtual bool initApplication (void);    // Determines flow from startup
    virtual bool configureRenderer (void);  // Displays the configuration screen - will be removed from the final program.

    // Called when the 2D menu system is loaded (and no 3D graphics are required/available).
    virtual bool loadLobby (void);
    virtual bool unloadLobby (void);

    // Called when the 3D graphics are loaded (and a game has been entered).
    virtual bool loadGame (void);
    virtual bool unloadGame (void);
    
    virtual void createCamera (void) = 0;
    virtual void createViewports (void) = 0;
    virtual void setupResources (void);
    virtual void loadResources (void);
    virtual void createFrameListener (void);
    virtual void createScene (void) = 0;     // This needs to be overridden to display anything
    virtual void destroyScene (void);
    virtual void updateParticleSystems (void);

    // Ogre::FrameListener overrides.
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    
    // Ogre::WindowEventListener overrides.
    virtual void windowResized (Ogre::RenderWindow* rw);
    virtual void windowClosed (Ogre::RenderWindow* rw);

    // OIS user input
    Input mUserInput;

    Ogre::Root*  mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    // OgreBites
    OgreBites::SdkCameraMan* mCameraMan;     // basic camera controller
    bool mCursorWasVisible;                  // Was the cursor visible before dialog appeared
    bool mDebrisVisible;
    bool mShutDown;
};


class SplashScreen : public Ogre::ResourceGroupListener
{
public:
    SplashScreen (Ogre::Root* root);
    ~SplashScreen (void);
    void draw (int width, int height);
    void clear (void);
    void updateProgressBar (int percent);

    // ResourceGroupListener Methods
    void resourceGroupScriptingStarted (const Ogre::String &groupName, size_t scriptCount);
    void scriptParseStarted (const Ogre::String &scriptName, bool &skipThisScript) {}
    void scriptParseEnded (const Ogre::String &scriptName, bool skipped);
    void resourceGroupScriptingEnded (const Ogre::String &groupName) {}
    void resourceGroupLoadStarted (const Ogre::String &groupName, size_t resourceCount) {}
    void resourceLoadStarted (const Ogre::ResourcePtr &resource) {}
    void resourceLoadEnded (void) {}
    void worldGeometryStageStarted (const Ogre::String &description) {}
    void worldGeometryStageEnded (void) {}
    void resourceGroupLoadEnded (const Ogre::String &groupName) {}
 	
private:
    void forceRedraw (void);

    int resourceTotal;
    int resourceCount;
    Ogre::Overlay*          splashOverlay;
	Ogre::OverlayContainer* splashContainer;
    Ogre::OverlayElement*   loadingFrame;
    Ogre::OverlayElement*   loadingBar;
    Ogre::Root* mRoot;
    //Ogre::OverlayElement*   loadingText;
};

#endif // #ifndef GRAPHICSCORE_H
