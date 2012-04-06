/**
 * @file    GraphicsCore.h
 * @brief     Configures the graphical settings and provides the common graphical functionality.
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
    mDebrisVisible(false),
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
    // Show the configuration dialog and returns true if the user clicks OK.
    if (mRoot->showConfigDialog())
        return true;
    return false;
}


void GraphicsCore::setupUserInput (void)
{
    OIS::ParamList     pl;
    size_t             windowHnd = 0;
    std::ostringstream windowHndStr;

    // Setup User Input, setting initial mouse clipping area.
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(make_pair("WINDOW", windowHndStr.str()));
    mUserInput.createInputSystem(pl);

    // Force the mouse clipping to be recalculated.
    windowResized(mWindow);
}


/// @brief  Creates a frame listener for the main window.
void GraphicsCore::createFrameListener (void)
{
    // Listener registration
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    mRoot->addFrameListener(this);
}


void GraphicsCore::updateParticleSystems (void)
{
    unsigned short i;
    // Check for stale emitters in the systems and cleanup
    for (i = 0; i < mSparkSystem->getNumEmitters(); i++)
        if (!mSparkSystem->getEmitter(i)->getEnabled())
            mSparkSystem->removeEmitter(i--);
    for (i = 0; i < mExplosionNucleusSystem->getNumEmitters(); i++)
        if (!mExplosionNucleusSystem->getEmitter(i)->getEnabled())
            mExplosionNucleusSystem->removeEmitter(i--);
    for (i = 0; i < mExplosionSmokeSystem->getNumEmitters(); i++)
        if (!mExplosionSmokeSystem->getEmitter(i)->getEnabled())
            mExplosionSmokeSystem->removeEmitter(i--);
    for (i = 0; i < mExplosionDebrisSystem->getNumEmitters(); i++)
        if (!mExplosionDebrisSystem->getEmitter(i)->getEnabled())
            mExplosionDebrisSystem->removeEmitter(i--);
}


void GraphicsCore::updateVIPLocation (int teamNumber, Ogre::Vector3 location)
{
    location.y += 4;
    if (teamNumber == 1)
        mVIPIcon[0]->setPosition(location);
    else
        mVIPIcon[1]->setPosition(location);
}


/// @brief  Generates an explosion.
void GraphicsCore::generateExplosion (Ogre::Vector3 location)
{
    unsigned short nucleusIndex = mExplosionNucleusSystem->getNumEmitters();
    unsigned short smokeIndex   = mExplosionSmokeSystem->getNumEmitters();
    unsigned short debrisIndex  = mExplosionDebrisSystem->getNumEmitters();

    mExplosionNucleusSystem->addEmitter("Point");
    mExplosionNucleusSystem->getEmitter(nucleusIndex)->setParameterList(mExplosionNucleusParams);
    mExplosionNucleusSystem->getEmitter(nucleusIndex)->setPosition(location);

    mExplosionSmokeSystem->addEmitter("Point");
    mExplosionSmokeSystem->getEmitter(smokeIndex)->setParameterList(mExplosionSmokeParams);
    mExplosionSmokeSystem->getEmitter(smokeIndex)->setPosition(location);

    mExplosionDebrisSystem->addEmitter("Point");
    mExplosionDebrisSystem->getEmitter(debrisIndex)->setParameterList(mExplosionDebrisParams);
    mExplosionDebrisSystem->getEmitter(debrisIndex)->setPosition(location);
    
    // Generate a sound (this isn't a particularly good way of doing it but it will work until a better method is available).
    static OgreOggSound::OgreOggISound* explosionSound = GameCore::mAudioCore->getSoundInstance(EXPLOSION, 0);
    GameCore::mAudioCore->playSoundOrRestart(explosionSound);
}

void GraphicsCore::generateSparks (Ogre::Vector3 location, Ogre::Vector3 direction)
{
    unsigned short sparkIndex = mSparkSystem->getNumEmitters();

    mSparkSystem->addEmitter("Point");
    mSparkSystem->getEmitter(sparkIndex)->setParameterList(mSparkParams);
    mSparkSystem->getEmitter(sparkIndex)->setPosition(location);
    mSparkSystem->getEmitter(sparkIndex)->setDirection(direction);
}


/// @brief  Removes everything from the scene.
void GraphicsCore::destroyScene (void)
{
}


/// @brief  Locates the resources using an external file. No resources are loaded at this stage.
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


/// @brief  Loads resources from the resources.cfg file into the ResourceGroup.
void GraphicsCore::loadResources (void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


/// @brief  Starts the graphics.
void GraphicsCore::go (void)
{
    srand(time(NULL));

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
    mResourcesCfg = "../../media/resources.cfg";
#ifdef _DEBUG
    mPluginsCfg = "plugins_d.cfg";
#else
    mPluginsCfg = "plugins.cfg";
#endif
    mRoot = new Ogre::Root(mPluginsCfg);
    setupResources();
    
    // Configure the renderer and exit if not configuration was provided (via the config dialog).
    if (!configureRenderer())
        return false;

    // Create a window.
    mWindow = mRoot->initialise(true, "Collision Domain");

    // Create the SceneManager.
    GameCore::mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    
    // Create the camera and viewport for viewing the scene
    createCamera();
    createViewports();

    // Create the splash screen (preloading its required resources in the process)
    SplashScreen splashScreen(mRoot);
    splashScreen.draw(mWindow->getWidth(), mWindow->getHeight());
    splashScreen.updateProgressBar(0, "Loading Resources...");
    Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(&splashScreen);

    // Load the remaining resources
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);   // Set default mipmap level
    loadResources();                    // Load resources
    GameCore::initialise(this, &splashScreen, 50);         // Initialise other game elements
    createScene();                      // Build the scene
    mSpawnScreen = NULL;
    createFrameListener();

    // Clear the splash screen
    splashScreen.clear();

#ifdef COLLISION_DOMAIN_CLIENT
    GameCore::mNetworkCore->AutoConnect( SERVER_PORT );
#endif

    return true;
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsCore::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    // Check for exit conditions.
    if (mWindow->isClosed())
        return false;
    if (mShutDown)
        return false;
    mUserInput.capture();
    if (mUserInput.mKeyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;

    // Feed the GUI the timestamping information.
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

    // Update the particle systems
    updateParticleSystems();
    // Rotate the VIP crowns (this should really be done somewhere else, but again waiting for a good place)
    mVIPIcon[0]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90 * evt.timeSinceLastFrame));
    mVIPIcon[1]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90 * evt.timeSinceLastFrame));

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




/*------------------------------ SPLASH SCREEN CLASS ------------------------------*/
SplashScreen::SplashScreen (Ogre::Root* root) : resourceTotal(0), resourceCount(0), mRoot(root)
{
    // Preload resources (for the splash screen)
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("ClientSplash");
}

SplashScreen::~SplashScreen (void)
{
}

void SplashScreen::draw (int width, int height)
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
    splashContainer->setDimensions(width, height);
    splashContainer->setMaterialName("CollisionDomain/SplashScreen");
    splashContainer->setPosition(0, 0);
    splashOverlay->add2D(splashContainer);

    // Add the loading bar frame to the splash screen.
    loadingFrame = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "LoadingFrame");
    loadingFrame->setMetricsMode(Ogre::GMM_PIXELS);
    loadingFrame->setDimensions(630, 40);
    loadingFrame->setMaterialName("CollisionDomain/LoadingFrame");
    loadingFrame->setPosition(width/2 - 630/2, height-50);
    splashContainer->addChild(loadingFrame);

    // Add the loading bar text to the splash screen.
    loadingText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "LoadingText");
    loadingText->setMetricsMode(Ogre::GMM_PIXELS);
    loadingText->setDimensions(600, 50);
    loadingText->setPosition(width/2, height-60);
    loadingText->setParameter("font_name", "DejaVuSans");
    loadingText->setParameter("char_height", "12");
    loadingText->setParameter("alignment", "center");
    loadingText->setColour(Ogre::ColourValue(0, 0, 0));
    loadingText->setCaption("");
    splashContainer->addChild(loadingText);

    // Add the loading bar to the splash screen.
    loadingBar = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "LoadingBar");
    loadingBar->setMetricsMode(Ogre::GMM_PIXELS);
    loadingBar->setDimensions(0, 10);
    loadingBar->setMaterialName("CollisionDomain/LoadingBar");
    loadingBar->setPosition(width/2 - 600/2, height-35);
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
    loadingBar->setDimensions(percent*6, 10);
    forceRedraw();
}

void SplashScreen::forceRedraw (void)
{
    mRoot->renderOneFrame();
}

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
        updateProgressBar(++resourceCount * ((float) (100 / resourceTotal) * 0.5f));
}