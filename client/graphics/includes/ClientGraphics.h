/**
 * @file    GraphicsCore.h
 * @brief     Configures the graphical settings and provides the common graphical functionality.
 */
#ifndef CLIENTGRAPHICS_H
#define CLIENTGRAPHICS_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "BigScreen.h"
#include "Lobby.h"
#include "SceneSetup.h"
#include "Camera.h"
#include "Input.h"

//#define COLLISION_DOMAIN_CLIENT

/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief     Builds the window and provides the necessary low level functions.
 */
class SpawnScreen;
class SceneSetup;

enum GraphicsState
{
    UNDEFINED,
    IN_LOBBY,
    //SPAWN_SCREEN,
    IN_GAME,
    BENCHMARKING,
    //SPECTATING_GAME,
    PROJECTOR
};

class ClientGraphics : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public SceneSetup
{
public:
    ClientGraphics (void);
    virtual ~ClientGraphics (void);
    virtual void go (void);
    void shutdown() { mShutDown = true; }
    void generateExplosion (Ogre::Vector3 location);
    void generateSparks (Ogre::Vector3 location, Ogre::Vector3 direction);
    void generateShrapnel (Ogre::Vector3 location, TeamID shrapnelTeam, float meanShrapnelQuantity=20, float planeOffset=-1.5f, Ogre::Vector3 planeNormal=Ogre::Vector3::UNIT_Y);
    void updateVIPLocation (TeamID teamID, Ogre::Vector3 location);

	//Setup the projector and set the correct game mode		
	void setupProjector();
    
    // Called when the 2D menu system is loaded (and no 3D graphics are required/available).
    virtual void loadLobby (void);
    virtual void unloadLobby (void);
    // Called when the 3D graphics are loaded (and a game has been entered).
    virtual void loadGame (void);
    virtual void unloadGame (void);

    // GraphicsApplication stuff
        void startBenchmark (uint8_t stage);

    Ogre::Camera*       mCamera;
    GameCamera *        mGameCam;
    //SpawnScreen*        mSpawnScreen;

	BigScreen*			mBigScreen;
	GraphicsState getGraphicsState();
    
    //Screen Specs
    int screenWidth;
    int screenHeight;

protected:
    // Called when the application is first started.
    virtual bool initApplication (void);    // Determines flow from startup
    virtual bool configureRenderer (void);  // Configures the render system used by ogre
    
    virtual void createCamera (void);
    virtual void createViewports (void);

    virtual void setupResources (void);
    virtual void loadResources (void);
    virtual void createFrameListener (void);
    virtual void createScene (void);     // This needs to be overridden to display anything
    
    virtual void setupUserInput (void);
    virtual void setupGUI (void);

    virtual void destroyScene (void);
    virtual void updateParticleSystems (void);

    // Ogre::FrameListener overrides.
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);

    virtual void updateBenchmark (const float timeSinceLastFrame);
    
    // Ogre::WindowEventListener overrides.
    virtual void windowResized (Ogre::RenderWindow* rw);
    virtual void windowClosed (Ogre::RenderWindow* rw);
    
    // Graphics Application stuff (to be moved)
        void finishBenchmark (uint8_t stage, float averageTriangles);
        uint8_t mBenchmarkStage;
    
    // Graphics state.
    GraphicsState mGraphicsState;
    Lobby* mLobby;

    // OIS user input.
    Input mUserInput;

    // Ogre bits and bobs.
    Ogre::Root*  mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    
    // OgreBites.
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
    void updateProgressBar (int percent, const Ogre::DisplayString& text);

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
    Ogre::OverlayElement*   loadingText;
    Ogre::Root* mRoot;
};

#endif // #ifndef GRAPHICSCORE_H
