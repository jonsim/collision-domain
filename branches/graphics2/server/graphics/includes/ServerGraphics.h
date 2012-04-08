/**
 * @file    GraphicsApplication.h
 * @brief     Adds objects to the graphics interface and contains the framelistener
 */
#ifndef SERVERGRAPHICS_H
#define SERVERGRAPHICS_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


// The maximum FPS for the server to run at. Note that, when possible, the server's FPS will *tend* to this number - higher and lower
// FPS are possible, especially for individual frames so it does NOT fix the frame interval, but rather causes the number of frames
// per second to tend to MAX_FPS.
// Defining this as 0, or undefining it alltogether will unlock the FPS (as of the time of writing this, 08/04/12, the unlocked server
// with singlethreadded ai/physics runs at around 4000 fps).
// While running the server at a high frame rate might seem attractive, it causes a very high and completely unnecessary load on both
// the graphics card and CPU for no visible gain (the physics runs at a fixed, independant frame rate, and none of the other gameplay
// elements require such frequent updates). Additional problems can occur as the timeSinceLastFrame value is very succeptible to
// hiccups at extremely high frame rates which can potentially cause fluctuations in the game performance.
// It is recommended to choose a number > 60, the current rate at which the physics runs, to ensure physics is simulated rather than 
// interpolated (which is less accurate and occurs when the timeStep > fixedTimeStep).
// TLDR: Leave on to lock the server's update speed and relinquish some CPU power and *significantly* reducing graphics card load,
//       set to 0 to unlock the fps for maximum performance.
#define MAX_FPS 100


/*-------------------- CLASS DEFINITIONS --------------------*/
class SceneSetup;

/**
 *  @brief  Manages the server's graphics (a console).
 */
class ServerGraphics : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public SceneSetup
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
    virtual void createFrameListener (void);
    
    virtual void createScene (void);
    virtual void setupGUI (void);
    virtual void setupUserInput (void);
    
    // Ogre::FrameListener overrides
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);
    
    // Ogre::WindowEventListener overrides.
    virtual void windowResized (Ogre::RenderWindow* rw);
    virtual void windowClosed (Ogre::RenderWindow* rw);

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    int usleep (long usec)
    {
        struct timeval tv;
        fd_set dummy;
        SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        FD_ZERO(&dummy);
        FD_SET(s, &dummy);
        tv.tv_sec = usec/1000000L;
        tv.tv_usec = usec%1000000L;
        return select(0, 0, 0, &dummy, &tv);
    }
#endif

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

class SplashScreen
{
public:
    SplashScreen (Ogre::Root* root);
    ~SplashScreen (void);
    void draw (void);
    void clear (void);
    void updateProgressBar (int percent);
    void updateProgressBar (int percent, const Ogre::DisplayString& text);

private:
    void forceRedraw (void);

    Ogre::Overlay*          splashOverlay;
    Ogre::OverlayContainer* splashContainer;
    Ogre::OverlayElement*   loadingBar;
    Ogre::OverlayElement*   loadingText;
    Ogre::Root* mRoot;
};

#endif // #ifndef SERVERGRAPHICS_H