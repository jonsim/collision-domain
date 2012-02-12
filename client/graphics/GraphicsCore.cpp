/**
 * @file	GraphicsCore.h
 * @brief 	Builds the window, loading the Ogre necessary libraries and providing 
 *          the Graphics Application with underlying functions to keep it tidy.
 *          Derived from the Ogre Tutorial Framework (BaseApplication.cpp).
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, initialising all resources.
GraphicsCore::GraphicsCore (void)
    : mRoot(0),
    mCamera(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCameraMan(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mSpawnScreen(0)
{
}


/// @brief  Deconstructor.
GraphicsCore::~GraphicsCore (void)
{
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
        mWindow = mRoot->initialise(true, "Collision Domain");
        return true;
    }

    return false;
}


/// @brief  Creates and positions the camera.
void GraphicsCore::createCamera (void)
{
    // Create the camera
    mCamera = GameCore::mSceneMgr->createCamera("PlayerCam");

    // Position it looking back along -Z
    mCamera->setPosition(Ogre::Vector3(0, 3, 60));
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
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

    // Register as a Window listener.
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	
	// Register as a Frame listener.
    mRoot->addFrameListener(this);
}


/// @brief  Removes everything from the scene.
void GraphicsCore::destroyScene (void)
{
}


/// @brief  Adds a single viewport that spans the entire window.
void GraphicsCore::createViewports (void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);

	// Set the background colour and match the aspect ratio to the window's.
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
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


/// @brief Creates a resource listener (whatever one of those is).
void GraphicsCore::createResourceListener (void)
{
}


/// @brief  Loads resources from the resources.cfg file into a ResourceGroup.
void GraphicsCore::loadResources (void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


/// @brief  Starts the graphics.
void GraphicsCore::go (void)
{
    srand ( time(NULL) );

	if (!initApplication() )
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
    mSpawnScreen = NULL;

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

    createFrameListener();

    return true;
};


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsCore::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// Check for exit conditions.
    if(mWindow->isClosed())
        return false;
    if(mShutDown)
        return false;
    mUserInput.capture();
    if (mUserInput.mKeyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;

	// Feed the GUI the timestamping information.
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

    return true;
}


/// @brief  Adjust mouse clipping area when the window is resized.
/// @param  rw  The window that has been resized.
void GraphicsCore::windowResized(Ogre::RenderWindow* rw)
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
void GraphicsCore::windowClosed(Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS
    if (rw == mWindow)
        mUserInput.destroyInputSystem();
}