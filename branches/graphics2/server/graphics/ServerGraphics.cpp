/**
 * @file    GraphicsApplication.cpp
 * @brief     Adds objects to the graphics interface.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"




/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor.
ServerGraphics::ServerGraphics (void) : mRoot(0),
                                        mCamera(0),
                                        mWindow(0),
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
    mRoot->startRendering();

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
    mWindow = mRoot->initialise(true, "Collision Domain Server");
    GameCore::mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    createCamera();
    createViewports();
    
    // Load the required resources
    // Create the splash screen (preloading its required resources in the process)
    SplashScreen splashScreen(mRoot);
    splashScreen.draw();
    //Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);   // Set default mipmap level
    //loadResources();                    // Load resources

    GameCore::initialise(&splashScreen, 0); // Initialise other game elements
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
    // Run the generic GUI setup
    SceneSetup::setupGUI();

    // Attach the GUI components
    GameCore::mGui->setupConsole();
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

bool ServerGraphics::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    static const float oneSecond = 1.0f / 60.0f;

    // Check for exit conditions.
    if (mWindow->isClosed())
        return false;
    if (mShutDown)
        return false;

    // Update the GUI.
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
    
    // Check if the network core is online
    if (!NetworkCore::bConnected)
        return true;

    // Capture the user input
    mUserInput.capture();
    
    // Process the networking. Sends client's input and receives data.
    GameCore::mNetworkCore->frameEvent();

    // Process the player pool. Perform updates on players.
    GameCore::mPlayerPool->frameEvent( evt );
    
    // Perform updates on AI players.
    GameCore::mAiCore->frameEvent( evt.timeSinceLastFrame );

    // Perform update on the powerups (basically manage spawning/deleting).
    GameCore::mPowerupPool->frameEvent( evt );

    // Currently unused updates
    //GameCore::mAudioCore->frameEvent(200);
    //GameCore::mGameplay->drawInfo();

    // LOCAL
    // get new snapshpot from control press - don't move the car though
    // get old snapshot received from server
    // create a new snapshot between new and old
    // apply new snapshot to player

    // REMOTE
    // get snapshot from server
    // apply to player
    
    // FUTURE
    // game will run x ticks behind the server
    // when a new snapshot is received, it should be in the client's future
    // interpolate based on snapshot timestamps

    // Perform Client Side Prediction.
    // Move any players who are out of sync

    /* Deal with all but local player (who's snapshots should be 0ms behind where this client thinks they are)
    for (i : otherPlayerIDs)
    {
        CarSnapshot *carSnapshot = getCarSnapshotIfExistsSincePreviousGet(int playerID);

        if (CSP.needsPushingBackIntoPosition(players[playerID], carSnapshot)
        {
            players[playerID].getCar()->restoreSnapshot(carSnapshot);
        }

        delete carSnapshot;
    }
    
    // Deal with the local player (who's snapshot will be x=latency ms behind the real thing)
    if (CSP.needsMePushedBack(players[clientID], carSnapshot))
    {
        // Calculate a snapshot which doesn't jolt the player harshly if it can be fixed with small movements
        CarSnapshot *latestPlayerSnapshot = getCarSnapshotIfExistsSincePreviousGet(int clientID);
        CarSnapshot *fixSnapshot = new CarSnapshot(...);

        players[clientID].getCar->restoreSnapshot(fixSnapshot);
    }
    */

    // Step physics. Minimum of 20 FPS (maxSubsteps=3) before physics becomes wrong.
    GameCore::mPhysicsCore->stepSimulation(evt.timeSinceLastFrame, 4, oneSecond);

    return true;
}

bool ServerGraphics::frameStarted (const Ogre::FrameEvent& evt)
{
    return true;
}

bool ServerGraphics::frameEnded (const Ogre::FrameEvent& evt)
{
    return true;
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