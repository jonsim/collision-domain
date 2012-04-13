/**
 * @file    GraphicsApplication.cpp
 * @brief     Adds objects to the graphics interface.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


// SERVER_FPS defines the rate at which the server processes requests and updates player states. This is NOT a maximum FPS, nor is it
// a stable fps, rather it is the target the server aims for. If there are not enough resources to achieve this FPS then the observed
// FPS will be lower, if there are an excess then the observed FPS will be around this value (due to the hyperactive nature of the
// scheduler it is often up to 20% higher than this figure). Define this to 0 to unlock the server's fps, causing it to run at the
// maximum speed (as of 08/04/12 the unlocked, single threadded server idles at ~420,000 FPS, with 100AI players this drops to 4,000).
// 
// GRAPHICS_FPS defines the rate the graphics updates at, this will never be more than the server's FPS, and in the case that the
// server updates close to the same speed this may drop. The graphics can NOT update faster than this. Define this to  0 to unlock the 
// graphics fps, causing it to run at the same speed as the SERVER_FPS (which can also be unlocked - though note that in this case the 
// bottle neck is the graphics card and will put a very heavy load on it).
// 
// PHYSICS_FPS defines the rate at which the physics updates at.
#define SERVER_FPS 100
#define GRAPHICS_FPS 30
#define PHYSICS_FPS 60




/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor.
ServerGraphics::ServerGraphics (void) : SceneSetup(),
                                        mRoot(0),
                                        mCamera(0),
                                        mResourcesCfg(Ogre::StringUtil::BLANK),
                                        mPluginsCfg(Ogre::StringUtil::BLANK),
                                        mCameraMan(0),
                                        mCursorWasVisible(false),
                                        mShutDown(false)
{
}

ServerGraphics::~ServerGraphics (void)
{
    // Destroy camera manager.
    if (mCameraMan)
        delete mCameraMan;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

/// @brief  Entry point for the application
void ServerGraphics::go (void)
{
    srand(time(NULL));

    // Initialise the application.
    if (!initApplication())
        return;

    // Enter the render loop.
    unsigned long usCurrentFrame = 0, usPreviousFrame = 0, numberOfFrames = 0;
    float sTimeSinceLastFrame = 0;
    mAverageFrameRate = 0;
#if SERVER_FPS > 0
    signed long usRemainingTime = 0;
    unsigned long usNextStateStep = 0;
    const unsigned long usStateStepSize    = 1000000 / SERVER_FPS;
#endif
#if GRAPHICS_FPS > 0
    unsigned long usNextGraphicsStep = 0;
    const unsigned long usGraphicsStepSize = 1000000 / GRAPHICS_FPS;
#endif

    mRoot->getRenderSystem()->_initRenderTargets();

    // Run the server at the server FPS, making the assumption that the graphics are both 
    // non-critical and update at a sufficiently lower rate than the state update rate,
    // extrapolating the graphical updates on top of the state updates.
#if SERVER_FPS > 0
    usNextStateStep    = mRoot->getTimer()->getMicroseconds() + usStateStepSize;
#endif
#if GRAPHICS_FPS > 0
    usNextGraphicsStep = mRoot->getTimer()->getMicroseconds() + usGraphicsStepSize;
#endif
    while (1)
    {
        // Calculate the time since the last frame was processed.
        usCurrentFrame = mRoot->getTimer()->getMicroseconds();
        sTimeSinceLastFrame = (float) (usCurrentFrame - usPreviousFrame) / 1000000.0f;
        if (numberOfFrames < 5000)
            mAverageFrameRate += ((1.0f / sTimeSinceLastFrame) - mAverageFrameRate) / ((float) ++numberOfFrames);
        else
            mAverageFrameRate += ((1.0f / sTimeSinceLastFrame) - mAverageFrameRate) / 5000;

        // Calculate the scheduled finish time of this step by adding the step size to
        // the previous scheduled finish. This accounts for both oversleep and undersleep
        // from the sleep at the end of the loop. If the previous scheduled finish is
        // already so far behind that we won't reach it this step, don't increment the
        // scheduled finish and instead use the previous one. This prevents 'losing time'
        // when the server can't run at its MAX_FPS, and stabilises the frame durations
        // on systems with high workload
        // Calculate when to have the next state step (the driving step).
#if SERVER_FPS > 0
        if (usNextStateStep < usCurrentFrame + usStateStepSize)
            usNextStateStep += usStateStepSize;
#endif

        // Update the gamestate
        updateState(sTimeSinceLastFrame);
        
        // Update the graphics, if this state step coincides with a graphics step.
#if GRAPHICS_FPS > 0
        if (usCurrentFrame > usNextGraphicsStep)
        {
#endif
            // Render the frame - pumping window messages to keep the system responsive and
            // drawing the GUI, exitting the loop if necessary.
            Ogre::WindowEventUtilities::messagePump();
            if (!mRoot->renderOneFrame())
                break;
#if GRAPHICS_FPS > 0
            usNextGraphicsStep += usGraphicsStepSize;
        }
#endif

        // Try to sleep for the remaining time.
        // We may wake up early or late, however the NextStep mechanic will force it to tend to
        // the target rate. When the SERVER_FPS is greater than it can reach, usRemainingTime 
        // will always be < 0 (as we are effectively losing time) and no sleeping will occur.
#if SERVER_FPS > 0
        usRemainingTime = usNextStateStep - mRoot->getTimer()->getMicroseconds();
        if (usRemainingTime > 0)
            usleep(usRemainingTime);
#endif
        usPreviousFrame = usCurrentFrame;
    }

    // Exit the render loop and clean up.
    GameCore::destroy();
}

/// @brief  Initialises the application.
/// @return Returns true if the initialisation was successful, false otherwise.
bool ServerGraphics::initApplication (void)
{
    // Select and load the relevant resources
    mResourcesCfg = "../../media/resources.cfg";
#ifdef _DEBUG
    mPluginsCfg = "plugins_d.cfg";
#else
    mPluginsCfg = "plugins.cfg";
#endif
    mRoot = new Ogre::Root(mPluginsCfg);
    setupResources();
    
    // Configure the renderer and exit if no configuration was provided.
    if (!configureRenderer())
        return false;

    // Create the window and viewport to go in it.
    NameValuePairList windowParameters;
    windowParameters["border"] = "fixed";
    mRoot->initialise(false);
    mWindow = mRoot->createRenderWindow("Collision Domain Server", 640, 480, false, &windowParameters);
    GameCore::mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    createCamera();
    createViewports();
    
    // Load the required resources
    // Create the splash screen (preloading its required resources in the process)
    SplashScreen splashScreen(mRoot);
    splashScreen.draw();
    //Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);   // Set default mipmap level
    //loadResources();                    // Load resources

    GameCore::initialise(this); // Initialise other game elements
    GameCore::load(&splashScreen, 0);
    GameCore::mNetworkCore->init(NULL);     // Initialise the server networking

    createScene();                          // Create the scene (in the server's case loading physics meshes)
    createFrameListener();                  // Create the frame listener to be used during rendering

    return true;
}

/// @brief  Configures the render system ogre uses.
/// @return Returns true if a valid configuration was selected, false otherwise.
bool ServerGraphics::configureRenderer (void)
{
    // Automatically initialise with the config dialog
    //if (mRoot->showConfigDialog())
        //return true;
    //return false;

    // Manually initialise
    RenderSystem* rs = NULL;
    RenderSystemList systems = Root::getSingleton().getAvailableRenderers();

    // Check if any render systems exist
    if (systems.empty())
        return false;
    // Check if OpenGL is one of those rendering systems (should be)
    for (RenderSystemList::iterator itr = systems.begin(); itr != systems.end(); itr++)
        if (!strcmp((*itr)->getName().c_str(), "OpenGL Rendering Subsystem"))
            rs = *itr;
    // If it wasn't, default to the first renderer
    if (rs == NULL)
    {
        OutputDebugString("OpenGL not found, defaulting to the first item.\n");
        rs = *systems.begin();
    }

    Root::getSingleton().setRenderSystem(rs);
    rs->setConfigOption("Video Mode", "640 x 480");
    rs->setConfigOption("Colour Depth", "32");
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("FSAA", "0");

    return true;
}

void ServerGraphics::createCamera (void)
{
    // Create the camera
    mCamera = GameCore::mSceneMgr->createCamera("PlayerCam");
    mCamera->setNearClipDistance(5);
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void ServerGraphics::createViewports (void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void ServerGraphics::setupResources (void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}

void ServerGraphics::loadResources (void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void ServerGraphics::createScene (void)
{
    // Add objects to the scene
    setupArenaNodes();
    setupArenaPhysics();

    // Setup the GUI
    setupGUI();

    // Setup Input
    setupUserInput();
}

/// @brief  Loads and sets up the resources required by CEGUI, creates a blank window layer and
///         adds the FPS counter to it.
void ServerGraphics::setupGUI (void)
{
    // Initialise the GUI renderer if it hasn't already been.
    SceneSetup::setupGUI();

    // Attach the GUI components
    GameCore::mGui->setupConsole(mGUIWindow);
}

void ServerGraphics::setupUserInput (void)
{
    OIS::ParamList     pl;
    size_t             windowHnd = 0;
    std::ostringstream windowHndStr;

    // Setup User Input, setting initial mouse clipping area.
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(make_pair("WINDOW", windowHndStr.str()));
    pl.insert(make_pair("w32_mouse", "DISCL_FOREGROUND"));    // Remove windows mouse exclusivity.
    pl.insert(make_pair("w32_mouse", "DISCL_NONEXCLUSIVE"));  // Remove windows mouse exclusivity.
    pl.insert(make_pair("x11_mouse_grab", "false"));          // Remove linux mouse exclusivity.
    mUserInput.createInputSystem(pl);

    // Force the mouse clipping area to be recalculated.
    windowResized(mWindow);
}

void ServerGraphics::createFrameListener (void)
{
    // Listener registration
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);  // Register as a Window listener.
    mRoot->addFrameListener(this);                                      // Register as a Frame listener.
}

bool ServerGraphics::frameStarted (const Ogre::FrameEvent& evt)
{
    return true;
}

bool ServerGraphics::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    // Check for exit conditions.
    if (mWindow->isClosed())
        return false;
    if (mShutDown)
        return false;

    // Update the GUI.
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
    
    return true;
}

bool ServerGraphics::frameEnded (const Ogre::FrameEvent& evt)
{
    return true;
}


void ServerGraphics::updateState (const float timeSinceLastFrame)
{
    static const float physicsTimeStep = 1.0f / PHYSICS_FPS;
    
    // Check if the network core is online
    if (!NetworkCore::bConnected)
        return;

    // Capture the user input
    mUserInput.capture();
    
    // Process the networking. Sends client's input and receives data.
    GameCore::mNetworkCore->frameEvent();

    // Process the player pool. Perform updates on players.
    GameCore::mPlayerPool->frameEvent(timeSinceLastFrame);
    
    // Perform updates on AI players.
    GameCore::mAiCore->frameEvent(timeSinceLastFrame);

    // Perform update on the powerups (basically manage spawning/deleting).
    GameCore::mPowerupPool->frameEvent(timeSinceLastFrame);

    // There was a giant ass comment here about client interpolation, see r409 and sooner to find it.
    // Step physics. Minimum of 20 FPS (maxSubsteps=3) before physics becomes wrong.
    GameCore::mPhysicsCore->stepSimulation(timeSinceLastFrame, 4, physicsTimeStep);
}


/// @brief  Called if the window is moved to give the console correct focus again.
/// @param  rw  The window that has been moved.
void ServerGraphics::windowMoved (Ogre::RenderWindow* rw)
{
    GameCore::mGui->giveConsoleFocus();
}


/// @brief  Called if the window has been tabbed into/out of to give the console correct focus again.
/// @param  rw  The window that has been moved.
void ServerGraphics::windowFocusChange (Ogre::RenderWindow* rw)
{
    GameCore::mGui->giveConsoleFocus();
}


/// @brief  Adjust mouse clipping area when the window is resized.
/// @param  rw  The window that has been resized.
void ServerGraphics::windowResized (Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mUserInput.mMouse->getMouseState();
    ms.width = width;
    ms.height = height;

    CEGUI::MouseCursor::getSingleton().hide();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    ShowCursor(true);
#else
    #error "Currently no non-windows method has been implemented to hide the hardware cursor."
#endif

    GameCore::mGui->giveConsoleFocus();
}


/// @brief  Closes a window, unattaching OIS before window shutdown (very important under Linux).
/// @param  rw  The window to close.
void ServerGraphics::windowClosed (Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS
    if (rw == mWindow)
        mUserInput.destroyInputSystem();
}


// Main function (entry point).
#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        ServerGraphics server;

        try
        {
            server.go();
        }
        catch (Ogre::Exception& e)
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif





/*------------------------------ SPLASH SCREEN CLASS ------------------------------*/
SplashScreen::SplashScreen (Ogre::Root* root) : mRoot(root)
{
    // Preload resources (for the splash screen)
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("ServerSplash");

    // Load fonts explicitly after resources as they are not be loaded on start up from the resource manager
    ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements())
        iter.getNext()->load();
}

SplashScreen::~SplashScreen (void)
{
}

void SplashScreen::draw (void)
{
    // Create the splash screen overlay
    splashOverlay = Ogre::OverlayManager::getSingleton().create("OVERLAY_SPLASH");
    splashOverlay->setZOrder(500);
    splashOverlay->show();

    // Create an overlay container and add the splash screen to it.
    splashContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "SplashScreen"));
    splashContainer->setMetricsMode(Ogre::GMM_PIXELS);
    splashContainer->setHorizontalAlignment(Ogre::GHA_LEFT);
    splashContainer->setVerticalAlignment(Ogre::GVA_TOP);
    splashContainer->setDimensions(640, 480);
    splashContainer->setMaterialName("CollisionDomain/ServerSplash");
    splashContainer->setPosition(0, 0);
    splashOverlay->add2D(splashContainer);
    
    // Add the loading bar text to the splash screen.
    loadingText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "LoadingText");
    loadingText->setMetricsMode(Ogre::GMM_PIXELS);
    loadingText->setDimensions(500, 50);
    loadingText->setPosition(320, 290);
    loadingText->setParameter("font_name", "DejaVuSans");
    loadingText->setParameter("char_height", "12");
    loadingText->setParameter("alignment", "center");
    loadingText->setColour(Ogre::ColourValue(1, 1, 1));
    loadingText->setCaption("");
    splashContainer->addChild(loadingText);

    // Add the loading bar to the splash screen.
    loadingBar = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "LoadingBar");
    loadingBar->setMetricsMode(Ogre::GMM_PIXELS);
    loadingBar->setDimensions(500, 20);
    loadingBar->setMaterialName("CollisionDomain/ServerLoadingBar");
    loadingBar->setPosition(70, 310);
    splashContainer->addChild(loadingBar);

    // Render the screen.
    forceRedraw();
}

void SplashScreen::clear (void)
{
    splashOverlay->clear();
    splashOverlay->hide();
}

void SplashScreen::updateProgressBar (int percent, const Ogre::DisplayString& text)
{
    loadingText->setCaption(text);
    updateProgressBar(percent);
}

void SplashScreen::updateProgressBar (int percent)
{
    loadingBar->setDimensions(percent*5, 20);
    forceRedraw();
}

void SplashScreen::forceRedraw (void)
{
    mRoot->renderOneFrame();
}
/*
void SplashScreen::resourceGroupScriptingStarted (const Ogre::String &groupName, size_t scriptCount)
{
    if (scriptCount > 10)
    {
        resourceTotal = scriptCount;
        resourceCount = 0;
    }
}

void SplashScreen::scriptParseEnded (const Ogre::String &scriptName, bool skipped)
{
    if (resourceTotal > 0)
        updateProgressBar(++resourceCount * ((float) (100 / resourceTotal)));
}*/