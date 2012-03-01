/**
 * @file    GraphicsApplication.cpp
 * @brief     Adds objects to the graphics interface.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"




/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor.
GraphicsApplication::GraphicsApplication (void)
{
}



/// @brief  Destructor.
GraphicsApplication::~GraphicsApplication (void)
{
}


/// @brief  Creates and positions the camera.
void GraphicsApplication::createCamera (void)
{
	//Create the bigscreen manager
	vpm = new ViewportManager(2, mWindow);
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

	mViewCam1->setPosition(Ogre::Vector3(0,0,80));
	mViewCam1->setNearClipDistance(5);
	mViewCam1->lookAt(Ogre::Vector3(0,0,-300));
	
	mViewCam2->setPosition(Ogre::Vector3(0,0,80));
	mViewCam2->setNearClipDistance(5);
	mViewCam2->lookAt(Ogre::Vector3(0,0,-300));
}


/// @brief  Adds a single viewport that spans the entire window.
void GraphicsApplication::createViewports (void)
{
	// Add viewports to the viewport manager.
	vpm->addViewport(mCamera,   true);
	vpm->addViewport(mViewCam1, false);
	vpm->addViewport(mViewCam2, false);
}


/// @brief  Creates the initial scene prior to the first render pass, adding objects etc.
void GraphicsApplication::createScene (void)
{
	// Save reference
	GameCore::mGraphicsApplication = this;
    
	// Setup the scene
    setupCompositorChain(mCamera->getViewport());
	setupShadowSystem();
    setupLightSystem();
    setupParticleSystem();
    setupArena();
    setupGUI();
}


void GraphicsApplication::setupGUI (void)
{
    // Run the generic GUI setup
    SceneSetup::setupGUI();

    // Attach the GUI components
    GameCore::mGui->setupConsole();
    GameCore::mGui->setupChatbox();
    
    // Setup the bigScreen map. As this is called *after* the 3D scene is setup the arena must
    // be fetched from the root scene node in this fashion in order to keep the code organised.
    bigScreen->setupMapView();
    try
    {
        Ogre::Entity*    ae = GameCore::mSceneMgr->getEntity("Arena");
        Ogre::SceneNode* an = GameCore::mSceneMgr->getSceneNode("ArenaNode");
        bigScreen->setMapCorner(an->getPosition());
        bigScreen->setMapSize(ae->getBoundingBox().getSize() * an->getScale());
    }
    catch (int e)
    {
        OutputDebugString("Exception caught while creating the bigScreen view, arena not fully initialised.\n");
    }
    GameCore::mGameplay->setupOverlay();
}


/// @brief  Passes the frame listener down to the GraphicsCore.
void GraphicsApplication::createFrameListener (void)
{
    GraphicsCore::createFrameListener();
    
    // The gamecore stuff should really be handled in a different manner - I will come back and change this later.
	GameCore::mNetworkCore->init(NULL);

	// Handle Game play (this isn't the place to do this, this will be moved).
	GameCore::mGameplay = new Gameplay();
	/*
	Team* t1 = GameCore::mGameplay->createTeam("Team1Name");
	Team* t2 = GameCore::mGameplay->createTeam("Team2Name");
	*/
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    static const float oneSecond = 1.0f / 60.0f;

    if (!GraphicsCore::frameRenderingQueued(evt))
        return false;
	if (!NetworkCore::bConnected)
		return true;
    
	// Update the big screen.
	bigScreen->updateMapView();
        
    // Process keyboard input and produce an InputState object from this.
    InputState* inputSnapshot;
	
	if( !GameCore::mGui->consoleVisible() && !GameCore::mGui->chatboxVisible() )
	{
		inputSnapshot = mUserInput.getInputState();
        mUserInput.processInterfaceControls();
	}
	else
	{
		// Don't want to capture any keys (typing things)
		inputSnapshot = new InputState(false, false, false, false, false);
	}
    
    // Process the networking. Sends client's input and receives data
    GameCore::mNetworkCore->frameEvent(inputSnapshot);

	// Process the player pool. Perform updates on other players
	GameCore::mPlayerPool->frameEvent( evt );

    // Apply controls the player (who will be moved on frameEnd and frameStart).
	GameCore::mPlayerPool->getLocalPlayer()->processControlsFrameEvent(inputSnapshot, evt.timeSinceLastFrame, oneSecond);
    //mPlayerPool->getLocalPlayer()->updateCameraFrameEvent(mUserInput.getMouseXRel(), mUserInput.getMouseYRel());

    GameCore::mPowerupPool->frameEvent( evt );

    //GameCore::mAudioCore->frameEvent(200);

	GameCore::mGameplay->drawInfo();

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

    // Cleanup frame specific objects so we don't rape memory. If you want to remember some, delete them later!
    delete inputSnapshot;

    // Minimum of 20 FPS (maxSubsteps=3) before physics becomes wrong
    GameCore::mPhysicsCore->stepSimulation(evt.timeSinceLastFrame, 4, (1.0f / 60.0f));

    return true;
}


/// @brief  Called once a frame every time processing for a frame has begun.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameStarted (const Ogre::FrameEvent& evt)
{
    return true;
}


/// @brief  Called once a frame every time processing for a frame has ended.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameEnded (const Ogre::FrameEvent& evt)
{
    return true;
}


// The following code is not understood. Does something for Win32: unknown. Best just leave it alone.
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
        GraphicsApplication app;

        try {
            app.go();
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
