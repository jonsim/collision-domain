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
    
    // Configure the renderer and exit if no configuration was provided (via the config dialog).
    if (!configureRenderer())
        return false;

    // Create the window and viewport to go in it.
    mWindow = mRoot->initialise(true, "Collision Domain Server");
    GameCore::mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    createCamera();
    createViewports();
    
    // Load the required resources
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);   // Set default mipmap level

    loadResources();                    // Load resources

    GameCore::initialise();             // Initialise other game elements
    GameCore::mNetworkCore->init(NULL); // Initialise the server networking
    setupGUI();                         // Initialise the GUI
    createFrameListener();              // Create the frame listener to be used during rendering
    return true;
}

/// @brief  Shows the configuration dialog
/// @return Returns true if the user enters a valid configuration, false otherwise.
bool ServerGraphics::configureRenderer (void)
{
    if (mRoot->showConfigDialog())
        return true;
    return false;
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

/// @brief  Loads and sets up the resources required by CEGUI, creates a blank window layer and
///         adds the FPS counter to it.
void ServerGraphics::setupGUI (void)
{
    // Bootstrap the GUI
    OutputDebugString("Bootstrapping CEGUI\n");
    mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    OutputDebugString("Initialising GUI\n");
    GameCore::mGui->initialiseGUI();
    OutputDebugString("Setting console\n");

    // go balls out and add the console
	GameCore::mGui->setupConsole();
	//GameCore::mGui->toggleConsole();
}

void ServerGraphics::createFrameListener (void)
{
    OIS::ParamList     pl;
    size_t             windowHnd = 0;
    std::ostringstream windowHndStr;

    // Setup User Input, setting initial mouse clipping area.
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mUserInput.createInputSystem(pl);
    windowResized(mWindow);

    // Listener registration
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);  // Register as a Window listener.
    mRoot->addFrameListener(this);                                      // Register as a Frame listener.
}

bool ServerGraphics::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    OutputDebugString("frq\n");
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
    GameCore::mPhysicsCore->stepSimulation(evt.timeSinceLastFrame, 4, (1.0f / 60.0f));

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
