/**
 * @file	GraphicsCore.h
 * @brief 	Builds the window, loading the Ogre necessary libraries and providing 
 *          the Graphics Application with underlying functions to keep it tidy.
 *          Derived from the Ogre Tutorial Framework (BaseApplication.cpp).
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>
//#define ADDITIONAL_SERVER_TRACKING_CAMERAS


/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, initialising all resources.
GraphicsCore::GraphicsCore (void)
    : mRoot(0),
    mCamera(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mCameraMan(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mSpawnScreen(0)
{
}


/// @brief  Deconstructor.
GraphicsCore::~GraphicsCore (void)
{
	// Destroy Ogre GUI (we are using CEGUI, we shouldn't be using Ogre Trays as well)...
    if (mTrayMgr)
		delete mTrayMgr;
	// Destroy camera manager.
    if (mCameraMan)
		delete mCameraMan;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}


/// @brief  Shows the configuration dialog and waits for the user to select their preferences
///         to initialise the system with.
///         You can skip this and use root.restoreConfig() to load configuration settings if 
///         you were sure there are valid ones saved in ogre.cfg
/// @return Whether or not the configuration was a success.
bool GraphicsCore::configureRenderer (void)
{
    // Show the configuration dialog and initialise the system (returns true if the user clicks OK).
    if (mRoot->showConfigDialog())
    {
        // Let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Collision Domain Server");
        return true;
    }

    return false;
}


/// @brief  Creates and positions the camera.
void GraphicsCore::createCamera (void)
{
	//Create the bigscreen manager
	vpm = new ViewportManager(2,mWindow);
	bigScreen = new BigScreen(vpm);

    // Create the cameras
    mCamera   = GameCore::mSceneMgr->createCamera("PlayerCam");
	mViewCam1 = GameCore::mSceneMgr->createCamera("ViewCam1");
	mViewCam2 = GameCore::mSceneMgr->createCamera("ViewCam2");
	bigScreen->addCamera(mViewCam1);
	bigScreen->addCamera(mViewCam2);

    // Position it looking back along -Z
    mCamera->setPosition(Ogre::Vector3(0,10,0));
    mCamera->lookAt(Ogre::Vector3(0,100,0));
    mCamera->setNearClipDistance(5);

    //mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller

	mViewCam1->setPosition(Ogre::Vector3(0,0,80));
	mViewCam1->setNearClipDistance(5);
	mViewCam1->lookAt(Ogre::Vector3(0,0,-300));
	
	mViewCam2->setPosition(Ogre::Vector3(0,0,80));
	mViewCam2->setNearClipDistance(5);
	mViewCam2->lookAt(Ogre::Vector3(0,0,-300));
}


/// @brief  Creates a frame listener for the main window.
void GraphicsCore::createFrameListener (void)
{
    OIS::ParamList     pl;
    size_t             windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	
    // Setup User Input, setting initial mouse clipping area.
    mUserInput.createInputSystem(pl);
    windowResized(mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	// Handle Game play (this isn't the place to do this, this will be moved).
	mGameplay = new Gameplay();
	Team* t1 = mGameplay->createTeam("Team1Name");
	Team* t2 = mGameplay->createTeam("Team2Name");

	// Setup the Ogre Trays GUI (we shouldn't be using this at all).
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mUserInput.mMouse, this);
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();
	
	// Add progress bars to the Ogre Trays GUI (we shouldn't be using this at all). 
	OgreBites::ProgressBar* mProgressBar = mTrayMgr->createProgressBar(OgreBites::TL_TOPLEFT, "PBarT1", t1->getName(), 200,200);
	mProgressBar->setProgress(1.0);
	OgreBites::ProgressBar* mProgressBar2 = mTrayMgr->createProgressBar(OgreBites::TL_TOPRIGHT, "PBarT2", t2->getName(), 200,200);
	mProgressBar2->setProgress(1.0);

	// Register as a Frame listener.
    mRoot->addFrameListener (this);
}


/// @brief  Removes everything from the scene.
void GraphicsCore::destroyScene (void)
{
}


/// @brief  Adds a single viewport that spans the entire window.
void GraphicsCore::createViewports (void)
{
	// Add viewports to the viewport manager.
	vpm->addViewport(mCamera,   true);	// This shouldn't exist anymore, will be purged.
	vpm->addViewport(mViewCam1, false);
	vpm->addViewport(mViewCam2, false);
}


/// @brief  Loads and configures all resources from an external file.
void GraphicsCore::setupResources (void)
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


/// @brief Creates a resource listener.
void GraphicsCore::createResourceListener (void)
{
}


/// @brief  Loads resources from the resources.cfg file into the ResourceGroup.
void GraphicsCore::loadResources (void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


/// @brief  Starts the graphics.
void GraphicsCore::go (void)
{
    srand ( time(NULL) );

	if (!initApplication())
		return;

    mRoot->startRendering();

    // clean up
    destroyScene();
	GameCore::destroy();
}


/// @brief  Attempts to load the resources and add them to the scene.
/// @return Whether or not the setup was successful (if a configuration was provided).
bool GraphicsCore::initApplication (void)
{
	// Select and load the relevant resources
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif
    mRoot = new Ogre::Root(mPluginsCfg);
    setupResources();
    
	// Configure the renderer and exit if not configuration was provided (via the config dialog).
    if (!configureRenderer())
		return false;

    // Init core classes, also init the SceneManager, in this case a generic one
    GameCore::initialise(this, mRoot->createSceneManager(Ogre::ST_GENERIC));

    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();
    
    // (Partially) init GameCore (this is completely the wrong place for this and will be purged).
	GameCore::mNetworkCore->init( NULL );
	Player *pPlayer = GameCore::mPlayerPool->getLocalPlayer();
	pPlayer->createPlayer( GameCore::mSceneMgr, CAR_BANGER, SKIN0, GameCore::mPhysicsCore );
	//pPlayer->attachCamera( mCamera );

    createFrameListener();

    return true;
};


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsCore::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
	// Check for exit conditions
    if (mWindow->isClosed())
        return false;
    if (mShutDown)
        return false;
    mUserInput.capture();
    if (mUserInput.mKeyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	// Update the big screen (this shouldn't be here).
	bigScreen->updateMapView();

	// Assign new VIP (this is completely the wrong place for this and will be purged).
	if (mUserInput.mKeyboard->isKeyDown(OIS::KC_V))
		mGameplay->setAllNewVIP();
	
    mTrayMgr->frameRenderingQueued(evt);

    return true;
}


/// @brief  Adjust mouse clipping area when the window is resized.
/// @param  rw  The window that has been resized.
void GraphicsCore::windowResized (Ogre::RenderWindow* rw)
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
void GraphicsCore::windowClosed (Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS
    if (rw == mWindow)
        mUserInput.destroyInputSystem();
}
