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
GraphicsCore::GraphicsCore(void)
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
GraphicsCore::~GraphicsCore(void)
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;

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
bool GraphicsCore::configure(void)
{
    // Show the configuration dialog and initialise the system
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Collision Domain Server");

        return true;
    }
    else
    {
        return false;
    }
}


/// @brief  Creates and positions the camera.
void GraphicsCore::createCamera(void)
{
	//Create the bigscreen manager
	vpm = new ViewportManager(2,mWindow);
	bigScreen = new BigScreen(vpm);
	//bigScreen->setupMapView();

    // Create the camera
    mCamera = GameCore::mSceneMgr->createCamera("PlayerCam");

	mViewCam1 = GameCore::mSceneMgr->createCamera("ViewCam1");
	mViewCam2 = GameCore::mSceneMgr->createCamera("ViewCam2");
	bigScreen->addCamera(mViewCam1);
	bigScreen->addCamera(mViewCam2);
#ifdef ADDITIONAL_SERVER_TRACKING_CAMERAS
	mViewCam3 = GameCore::mSceneMgr->createCamera("ViewCam3");
	mViewCam4 = GameCore::mSceneMgr->createCamera("ViewCam4");
	mViewCam5 = GameCore::mSceneMgr->createCamera("ViewCam5");
	mViewCam6 = GameCore::mSceneMgr->createCamera("ViewCam6");
	mViewCam7 = GameCore::mSceneMgr->createCamera("ViewCam7");
	mViewCam8 = GameCore::mSceneMgr->createCamera("ViewCam8");
	mViewCamBlank = GameCore::mSceneMgr->createCamera("ViewCamBlank");
	bigScreen->addCamera(mViewCam3);
	bigScreen->addCamera(mViewCam4);
	bigScreen->addCamera(mViewCam5);
	bigScreen->addCamera(mViewCam6);
	bigScreen->addCamera(mViewCam7);
	bigScreen->addCamera(mViewCam8);
#endif

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,550,-100));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,0));
    mCamera->setNearClipDistance(5);

    //mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller

	mViewCam1->setPosition(Ogre::Vector3(0,0,80));
	mViewCam1->setNearClipDistance(5);
	mViewCam1->lookAt(Ogre::Vector3(0,0,-300));
	
	mViewCam2->setPosition(Ogre::Vector3(0,0,80));
	mViewCam2->setNearClipDistance(5);
	mViewCam2->lookAt(Ogre::Vector3(0,0,-300));
#ifdef ADDITIONAL_SERVER_TRACKING_CAMERAS
	mViewCam3->setPosition(Ogre::Vector3(0,0,80));
	mViewCam3->setNearClipDistance(5);
	mViewCam3->lookAt(Ogre::Vector3(0,0,-300));

	mViewCam4->setPosition(Ogre::Vector3(0,0,80));
	mViewCam4->setNearClipDistance(5);
	mViewCam4->lookAt(Ogre::Vector3(0,0,-300));

	mViewCam5->setPosition(Ogre::Vector3(0,0,80));
	mViewCam5->setNearClipDistance(5);
	mViewCam5->lookAt(Ogre::Vector3(0,0,-300));

	mViewCam6->setPosition(Ogre::Vector3(0,0,80));
	mViewCam6->setNearClipDistance(5);
	mViewCam6->lookAt(Ogre::Vector3(0,0,-300));

	mViewCam7->setPosition(Ogre::Vector3(0,0,80));
	mViewCam7->setNearClipDistance(5);
	mViewCam7->lookAt(Ogre::Vector3(0,0,-300));

	mViewCam8->setPosition(Ogre::Vector3(0,0,80));
	mViewCam8->setNearClipDistance(5);
	mViewCam8->lookAt(Ogre::Vector3(0,0,-300));
#endif
}


/// @brief  Creates a frame listener for the main window.
void GraphicsCore::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    // Setup User Input
    mUserInput.createInputSystem(pl);
    
    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mUserInput.mMouse, this);
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();

    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("Player speed:");
    items.push_back("Player rotation:");
    items.push_back("sin(rotation):");
    items.push_back("cos(rotation):");
    items.push_back("Player location:");
    items.push_back("Time since last frame:");
    //items.push_back("cam.oX");
    //items.push_back("cam.oY");
    //items.push_back("cam.oZ");
    //items.push_back("");
    //items.push_back("Filtering");
    //items.push_back("Poly Mode");

    mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 400, items);
    //mDetailsPanel->setParamValue(9, "Bilinear");
    //mDetailsPanel->setParamValue(10, "Solid");
    mDetailsPanel->hide();

	//Handle Game play
	mGameplay = new Gameplay();
	Team* t1 = mGameplay->createTeam("Team1Name");
	Team* t2 = mGameplay->createTeam("Team2Name");

	OgreBites::ProgressBar* mProgressBar = mTrayMgr->createProgressBar(OgreBites::TL_TOPLEFT, "PBarT1", t1->getName(), 200,200);
	mProgressBar->setProgress(1.0);
	OgreBites::ProgressBar* mProgressBar2 = mTrayMgr->createProgressBar(OgreBites::TL_TOPRIGHT, "PBarT2", t2->getName(), 200,200);
	mProgressBar2->setProgress(1.0);


    mRoot->addFrameListener(this);
}


/// @brief  Removes everything from the scene.
void GraphicsCore::destroyScene(void)
{
}


/// @brief  Adds a single viewport that spans the entire window.
void GraphicsCore::createViewports(void)
{
	//ViewportManager* vpm = new ViewportManager(8,mWindow);
	vpm->addViewport(mCamera,true);
	vpm->addViewport(mViewCam1,false);
	vpm->addViewport(mViewCam2,false);
#ifdef ADDITIONAL_SERVER_TRACKING_CAMERAS
	vpm->addViewport(mViewCam3,false);
	vpm->addViewport(mViewCam4,false);
	vpm->addViewport(mViewCam5,false);
	vpm->addViewport(mViewCam6,false);
	vpm->addViewport(mViewCam7,false);
	vpm->addViewport(mViewCam8,false);
#endif
}


/// @brief  Loads and configures all resources from an external file.
void GraphicsCore::setupResources(void)
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
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}


/// @brief Creates a resource listener (whatever one of those is).
void GraphicsCore::createResourceListener(void)
{

}


/// @brief  Loads resources from the resources.cfg file into a ResourceGroup.
void GraphicsCore::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


/// @brief  Starts the graphics.
void GraphicsCore::go(void)
{
    srand ( time(NULL) );

#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

	if( !setup() )
		return;

    mRoot->startRendering();

    // clean up
    destroyScene();
	GameCore::destroy();
}


/// @brief  Attempts to load the resources and add them to the scene.
/// @return Whether or not the setup was successful (if a configuration was provided).
bool GraphicsCore::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg);

    setupResources();
    
    bool carryOn = configure();
    if (!carryOn) return false;

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
    
    // MORE GAMECORE RELATED INIT
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
bool GraphicsCore::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;
    
	bigScreen->updateMapView();

    //Need to capture/update each device
    mUserInput.capture();

    if (mUserInput.mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	//Assign new 
	if (mUserInput.mKeyboard->isKeyDown(OIS::KC_V))
	{
		mGameplay->setAllNewVIP();
	}

	//Print out local player current position
	if (mUserInput.mKeyboard->isKeyDown(OIS::KC_I))
	{
		Player* tmpLocalPlayer = GameCore::mPlayerPool->getLocalPlayer();
		btVector3 tmpLocalPlayerVector = tmpLocalPlayer->getCar()->getCarSnapshot()->mPosition;
		std::stringstream curPosDebugString;
		curPosDebugString << "X: " << tmpLocalPlayerVector.getX();
		curPosDebugString << " Y: " << tmpLocalPlayerVector.getY();
		curPosDebugString << " Z: " << tmpLocalPlayerVector.getZ();
		curPosDebugString << "\n";
		

		//OutputDebugString(curPosDebugString.str().c_str());
	}

	
    mTrayMgr->frameRenderingQueued(evt);

    // print debug output if necessary
    if (!mTrayMgr->isDialogVisible())
    {
        //mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
        if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
        {
            mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
            mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
            mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
            mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
            mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
            mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
            mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
        }
    }
    return true;
}


/// @brief  Adjust mouse clipping area.
/// @param  rw  The window to resize.
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
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        mUserInput.destroyInputSystem();
    }
}
