/**
 * @file    GraphicsCore.h
 * @brief     Configures the graphical settings and provides the common graphical functionality.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, initialising all resources.
ClientGraphics::ClientGraphics (void)
    : mRoot(0),
    mCamera(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCameraMan(0),
    mCursorWasVisible(false),
    mDebrisVisible(false),
    mShutDown(false),
    mSpawnScreen(0),
    mGraphicsState(UNDEFINED),
    mBenchmarkRunning(false)
{
}


/// @brief  Deconstructor.
ClientGraphics::~ClientGraphics (void)
{
    // Destroy camera manager.
    if (mCameraMan)
        delete mCameraMan;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}


/// @brief  Starts the graphics.
void ClientGraphics::go (void)
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
bool ClientGraphics::initApplication (void)
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

    // Create the window and scenemanager to control it.
    mWindow = mRoot->initialise(true, "Collision Domain");
    GameCore::mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    
    // Create the camera and viewport for viewing the scene
    createCamera();
    createViewports();

    // Load the lobby
    loadLobby();

    // Load the game
    //loadGame();

    createFrameListener();

    return true;
}


/// @brief  Locates the resources using an external file. No resources are loaded at this stage.
void ClientGraphics::setupResources (void)
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


/// @brief  Shows the configuration dialog and waits for the user to select their preferences
///         to initialise the system with.
///         You can skip this and use root.restoreConfig() to load configuration settings if 
///         you were sure there are valid ones saved in ogre.cfg
/// @return Whether or not the configuration was a success.
bool ClientGraphics::configureRenderer (void)
{
    // Show the configuration dialog and returns true if the user clicks OK.
    if (mRoot->showConfigDialog())
        return true;
    return false;
}


/// @brief  Creates and positions the camera.
void ClientGraphics::createCamera (void)
{
    // Create the camera
    mCamera = GameCore::mSceneMgr->createCamera("PlayerCam");

    // Position it looking back along -Z
    mCamera->setPosition(Ogre::Vector3(0, 3, 60));
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(0.5);
    mCamera->setFarClipDistance(2500);

    //mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}


/// @brief  Adds a single viewport that spans the entire window.
void ClientGraphics::createViewports (void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);

	// Set the background colour and match the aspect ratio to the window's.
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}


void ClientGraphics::loadLobby (void)
{
    // Set the graphics state
    mGraphicsState = IN_LOBBY;

    // First we need to load CEGUI and it's resources by running the generic setup.
    SceneSetup::setupGUI();
    setupUserInput();

    //GameCore::mGui->setupLobby(mWindow->getWidth(), mWindow->getHeight());
}


void ClientGraphics::unloadLobby (void)
{
    // Set the graphics state
    mGraphicsState = UNDEFINED;
}


void ClientGraphics::loadGame (void)
{
    // Set the graphics state
    mGraphicsState = PLAYING_GAME;

    // Create the splash screen (preloading its required resources in the process)
    SplashScreen splashScreen(mRoot);
    splashScreen.draw(mWindow->getWidth(), mWindow->getHeight());
    splashScreen.updateProgressBar(0, "Loading Resources...");
    Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(&splashScreen);

    // Load the remaining resources.
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    loadResources();

    // Initialise the other game elements.
    GameCore::initialise(this, &splashScreen, 50);

    // Build the scene.
    createScene();
    mSpawnScreen = NULL;

    // Clear the splash screen.
    splashScreen.clear();

    // Auto connect to the first server you can find.
    GameCore::mNetworkCore->AutoConnect( SERVER_PORT );
}


void ClientGraphics::unloadGame (void)
{
    // Set the graphics state
    mGraphicsState = UNDEFINED;
}


/// @brief  Loads resources from the resources.cfg file into the ResourceGroup.
void ClientGraphics::loadResources (void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


/// @brief  Creates the initial scene prior to the first render pass, adding objects etc.
void ClientGraphics::createScene (void)
{
	// Setup the scene environment.
    setupCompositorChain(mCamera->getViewport());
	setupShadowSystem();
    setupLightSystem();
    setupParticleSystem();

    // Add objects to the scene.
    setupArenaNodes();
    setupArenaPhysics();
    setupArenaGraphics();

    // Setup the GUI.
    setupGUI();

    // Setup the input.
    setupUserInput();

    // Setup the scene resources.
    setupMeshDeformer();
}


void ClientGraphics::setupGUI (void)
{
    // Run the generic GUI setup
    SceneSetup::setupGUI();

    // Attach the GUI components
    GameCore::mGui->setupFPSCounter();
    GameCore::mGui->setupConnectBox();
    GameCore::mGui->setupConsole();
    GameCore::mGui->setupChatbox();
    GameCore::mGui->setupOverlays();

    GameCore::mGameplay->setupOverlay();
}


void ClientGraphics::setupUserInput (void)
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
void ClientGraphics::createFrameListener (void)
{
    // Listener registration
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    mRoot->addFrameListener(this);
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool ClientGraphics::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    // Check for exit conditions.
    if (mWindow->isClosed())
        return false;
    if (mShutDown)
        return false;

    if (mGraphicsState == IN_LOBBY)
    {
        mUserInput.capture();

        CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
    }
    else if (mGraphicsState == PLAYING_GAME)
    {
        static const float physicsTimeStep = 1.0f / 60.0f;

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
    
        // Check for benchmarking
	    if (mBenchmarkRunning)
	    {
		    static float    benchmarkProgress = 0;
		    static float    CATriangles       = 0;
		    static uint16_t CAi               = 0;
            benchmarkProgress += evt.timeSinceLastFrame;
		    CATriangles += ((float) (mWindow->getTriangleCount() - CATriangles)) / ((float) (++CAi));
		    // stop the benchmark after 8 seconds
		    if (benchmarkProgress > 8)
		    {
			    CAi               = 0;
			    benchmarkProgress = 0;
			    mBenchmarkRunning = false;
			    finishBenchmark(mBenchmarkStage, CATriangles);
		    }

		    // rotate the camera
            GameCore::mPlayerPool->getLocalPlayer()->updateCameraFrameEvent(500 * evt.timeSinceLastFrame, 0.0f, 0.0f, evt.timeSinceLastFrame);

		    // update fps counter
		    float avgfps = mWindow->getAverageFPS(); // update fps
		    CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
		    char szFPS[16];
		    sprintf(szFPS, "FPS: %.2f", avgfps);
		    fps->setText(szFPS);

		    // dont do any of the non-graphics bullshit
		    return true;
	    }

        // Collect input
	    InputState *inputSnapshot = mUserInput.getInputState();
        mUserInput.processInterfaceControls();
    
        // Process the networking. Sends client's input and receives data
        GameCore::mNetworkCore->frameEvent(inputSnapshot);

	    if (NetworkCore::bConnected)
	    {
            // Process the player pool. Perform updates on other players
            GameCore::mPlayerPool->frameEvent(evt.timeSinceLastFrame);
            if (GameCore::mPlayerPool->getLocalPlayer()->getCar() != NULL)
            {
                GameCore::mPlayerPool->getLocalPlayer()->updateLocalGraphics();
                GameCore::mAudioCore->frameEvent(GameCore::mPlayerPool->getLocalPlayer()->getCar()->getRPM());
                GameCore::mGui->updateCounters();
                GameCore::mGui->updateSpeedo();
            }

	    }

        /*  NOTE TO SELF (JAMIE)
            Client doesn't want to do PowerupPool::frameEvent() when powerups are networked
            All powerup removals handled by the server's collision events
            In fact, client probably shouldn't register any collision callbacks for powerups
        */
        GameCore::mPowerupPool->frameEvent(evt.timeSinceLastFrame);

        // FUTURE
        // game will run x ticks behind the server
        // when a new snapshot is received, it should be in the client's future
        // interpolate based on snapshot timestamps

    

        // Minimum of 30 FPS (maxSubsteps=2) before physics becomes wrong
        GameCore::mPhysicsCore->stepSimulation(evt.timeSinceLastFrame, 2, physicsTimeStep);
	
	    //Draw info items
	    GameCore::mGameplay->drawInfo();

    
	    // Apply controls the player (who will be moved on frameEnd and frameStart).
        if (NetworkCore::bConnected)
        {
	        if (GameCore::mPlayerPool->getLocalPlayer()->getCar() != NULL)
	        {
		        GameCore::mPlayerPool->getLocalPlayer()->processControlsFrameEvent(inputSnapshot, evt.timeSinceLastFrame, physicsTimeStep);
		        GameCore::mPlayerPool->getLocalPlayer()->updateCameraFrameEvent(mUserInput.getMouseXRel(), mUserInput.getMouseYRel(), mUserInput.getMouseZRel(), evt.timeSinceLastFrame);
	        }
        }

        // Cleanup frame specific objects.
        delete inputSnapshot;
    }

    return true;
}


/// @brief  Called once a frame every time processing for a frame has begun.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool ClientGraphics::frameStarted (const Ogre::FrameEvent& evt)
{
    return true;
}


/// @brief  Called once a frame every time processing for a frame has ended.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool ClientGraphics::frameEnded (const Ogre::FrameEvent& evt)
{
    return true;
}


void ClientGraphics::updateParticleSystems (void)
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


void ClientGraphics::updateVIPLocation (int teamNumber, Ogre::Vector3 location)
{
    location.y += 4;
    if (teamNumber == 1)
        mVIPIcon[0]->setPosition(location);
    else
        mVIPIcon[1]->setPosition(location);
}


/// @brief  Generates an explosion.
void ClientGraphics::generateExplosion (Ogre::Vector3 location)
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


void ClientGraphics::generateSparks (Ogre::Vector3 location, Ogre::Vector3 direction)
{
    unsigned short sparkIndex = mSparkSystem->getNumEmitters();

    mSparkSystem->addEmitter("Point");
    mSparkSystem->getEmitter(sparkIndex)->setParameterList(mSparkParams);
    mSparkSystem->getEmitter(sparkIndex)->setPosition(location);
    mSparkSystem->getEmitter(sparkIndex)->setDirection(direction);
}


/// @brief  Removes everything from the scene.
void ClientGraphics::destroyScene (void)
{
}


/// @brief  Adjust mouse clipping area when the window is resized.
/// @param  rw  The window that has been resized.
void ClientGraphics::windowResized (Ogre::RenderWindow* rw)
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
void ClientGraphics::windowClosed (Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS
    if (rw == mWindow)
        mUserInput.destroyInputSystem();
}


/* ---------- Benchmarking ---------- */


void ClientGraphics::startBenchmark (uint8_t stage)
{
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	Ogre::Viewport* vp = mCamera->getViewport();
	switch (stage)
	{
		case 0:	// all off
			OutputDebugString("Starting benchmark...\n");
			cm.removeCompositor(vp, "HDR");
			cm.removeCompositor(vp, "Bloom");
			cm.removeCompositor(vp, "MotionBlur");
			cm.removeCompositor(vp, "RadialBlur");
			break;
		case 1: // just hdr on
			cm.addCompositor(vp, "HDR");
			loadHDR(vp, 0);
			break;
		case 2: // just bloom on
			cm.removeCompositor(vp, "HDR");
			cm.addCompositor(vp, "Bloom");
			loadBloom(vp, 0, 0.15f, 1.0f);
			break;
		case 3: // just MotionBlur on
			cm.removeCompositor(vp, "Bloom");
			cm.addCompositor(vp, "MotionBlur");
			loadMotionBlur(vp, 0, 0.1f);
			break;
		case 4: // just RadialBlur on
			cm.removeCompositor(vp, "MotionBlur");
			cm.addCompositor(vp, "RadialBlur");
			cm.setCompositorEnabled(vp, "RadialBlur", true);
			break;
		case 5: // all on
			cm.addCompositor(vp, "HDR");
			loadHDR(vp, 0);
			cm.addCompositor(vp, "Bloom");
			loadBloom(vp, 0, 0.15f, 1.0f);
			cm.addCompositor(vp, "MotionBlur");
			loadMotionBlur(vp, 0, 0.1f);
			break;
	}
	
	mWindow->resetStatistics();
	mBenchmarkStage = stage;
	mBenchmarkRunning = true;
}

void ClientGraphics::finishBenchmark (uint8_t stage, float averageTriangles)
{
	static float r[6];
	static float triangles;
	r[stage] = mWindow->getAverageFPS();
	if (stage == 0)
		triangles = averageTriangles;

	if (stage == 5)
	{
		std::ofstream rFile;
		rFile.open("BenchmarkResults.txt", std::ios::out | std::ios::trunc);
		rFile << std::fixed;
		rFile << "              BENCHMARKING RESULTS\n";
		rFile << " Average triangles per frame = " << std::setprecision(0) << triangles << "\n\n";
		rFile << "+-----+-------+-----+-----+-------+-------+\n";
		rFile << "| HDR | Bloom | MoB | RaB |  FPS  |  DIF  |\n";
		rFile << "+-----+-------+-----+-----+-------+-------+\n";
		rFile.precision(2);
		rFile << "|  0  |   0   |  0  |  0  | " << std::setw(5) << std::setfill(' ') << r[0] << " | 00.00 |\n";
		rFile << "|  1  |   0   |  0  |  0  | " << std::setw(5) << std::setfill(' ') << r[1] << " | " << std::setw(5) << std::setfill(' ') << r[0] - r[1] << " |\n";
		rFile << "|  0  |   1   |  0  |  0  | " << std::setw(5) << std::setfill(' ') << r[2] << " | " << std::setw(5) << std::setfill(' ') << r[0] - r[2] << " |\n";
		rFile << "|  0  |   0   |  1  |  0  | " << std::setw(5) << std::setfill(' ') << r[3] << " | " << std::setw(5) << std::setfill(' ') << r[0] - r[3] << " |\n";
		rFile << "|  0  |   0   |  0  |  1  | " << std::setw(5) << std::setfill(' ') << r[4] << " | " << std::setw(5) << std::setfill(' ') << r[0] - r[4] << " |\n";
		rFile << "|  1  |   1   |  1  |  1  | " << std::setw(5) << std::setfill(' ') << r[5] << " | " << std::setw(5) << std::setfill(' ') << r[0] - r[5] << " |\n";
		rFile << "+-----+-------+-----+-----+-------+-------+\n";
		rFile.close();
		OutputDebugString("Benchmark complete. See $(OGRE_HOME)/bin/debug/BenchmarkResults.txt for the results.\n");
		Ogre::CompositorManager::getSingleton().removeCompositor(mCamera->getViewport(), "HDR");
	}
	else
	{
		startBenchmark(stage+1);
	}
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




/*------------------------------ MAIN FUNCTION ------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        ClientGraphics application;

        try {
            application.go();
        } catch( Ogre::Exception& e ) {
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
