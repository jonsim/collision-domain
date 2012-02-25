/**
 * @file    GraphicsApplication.cpp
 * @brief     Adds objects to the graphics interface.
 *          Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
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
	mGuiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	GameCore::mGui->setupGUI();

    setupLighting(1);
    setupArena();

    // Load the ninjas
    Ogre::Entity* ninjaEntity = GameCore::mSceneMgr->createEntity("Ninja", "ninja.mesh");
    ninjaEntity->setCastShadows(true);
    Ogre::SceneNode* ninjaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    ninjaNode->attachObject(ninjaEntity);
    ninjaNode->scale(0.2f, 0.2f, 0.2f);
    ninjaNode->translate(100.0f,0,0);

	GameCore::mGui->displayConsole();
	GameCore::mGui->displayChatbox();
	GameCore::mGraphicsCore->bigScreen->setupMapView();
	GameCore::mGameplay->setupOverlay();
}


/// @brief  Adds and configures lights to the scene.
/// @param  mode	The lighting mode to use. 0 = Morning, 1 = Noon, 2 = Stormy.
void GraphicsApplication::setupLighting (uint8_t mode)
{
	Ogre::Degree sunRotation;	// rotation horizontally (yaw) from +x axis
	Ogre::Degree sunPitch;		// rotation downwards (pitch) from horizontal
	float sunBrightness[4];	// RGBA
	float   sunSpecular[4];	// RGBA
	float   sunAmbience[4];	// RGBA
	std::string skyBoxMap;
	float sf; // scaling factor

	// Set lighting constants
	if (mode < 1)	// Morning
	{
		sunRotation = -170;
		sunPitch = 18;
		sunBrightness[0] = 251;
		sunBrightness[1] = 215;
		sunBrightness[2] = 140;
		sunBrightness[3] = 800;
		sunSpecular[0] = 251;
		sunSpecular[1] = 215;
		sunSpecular[2] = 140;
		sunSpecular[3] = 400;
		sunAmbience[0] = 143;
		sunAmbience[1] = 176;
		sunAmbience[2] = 214;
		sunAmbience[3] = 300;
		skyBoxMap = "Examples/MorningSkyBox";
	}
	else if (mode == 1) // Noon
	{
		sunRotation = 43;
		sunPitch = 57;
		sunBrightness[0] = 242;
		sunBrightness[1] = 224;
		sunBrightness[2] = 183;
		sunBrightness[3] = 850;
		sunSpecular[0] = 242;
		sunSpecular[1] = 224;
		sunSpecular[2] = 183;
		sunSpecular[3] = 425;
		sunAmbience[0] = 105;
		sunAmbience[1] = 150;
		sunAmbience[2] = 186;
		sunAmbience[3] = 800;
		skyBoxMap = "Examples/CloudyNoonSkyBox";
	}
	else // Stormy
	{
		sunRotation = -55;
		sunPitch = 60;
		sunBrightness[0] = 240;
		sunBrightness[1] = 252;
		sunBrightness[2] = 255;
		sunBrightness[3] = 200;
		sunSpecular[0] = 240;
		sunSpecular[1] = 252;
		sunSpecular[2] = 255;
		sunSpecular[3] = 100;
		sunAmbience[0] = 146;
		sunAmbience[1] = 149;
		sunAmbience[2] = 155;
		sunAmbience[3] = 300;
		skyBoxMap = "Examples/StormySkyBox";
		
		// add rain (this could be improved)
		Ogre::ParticleSystem* rainSystem = GameCore::mSceneMgr->createParticleSystem("Rain", "Examples/RainSmall");
		GameCore::mSceneMgr->getRootSceneNode()->attachObject(rainSystem);
	}
	
	// Setup the lighting colours
	sf = (1.0f / 255.0f) * (sunAmbience[3] / 1000.0f);
	Ogre::ColourValue sunAmbienceColour   = Ogre::ColourValue(sunAmbience[0]   * sf, sunAmbience[1]   * sf, sunAmbience[2]   * sf);
	sf = (1.0f / 255.0f) * (sunBrightness[3] / 1000.0f);
	Ogre::ColourValue sunBrightnessColour = Ogre::ColourValue(sunBrightness[0] * sf, sunBrightness[1] * sf, sunBrightness[2] * sf);
	sf = (1.0f / 255.0f) * (sunSpecular[3] / 1000.0f);
	Ogre::ColourValue sunSpecularColour   = Ogre::ColourValue(sunSpecular[0]   * sf, sunSpecular[1]   * sf, sunSpecular[2]   * sf);

	// Calculate the sun direction (using rotation matrices).
	Ogre::Real cos_pitch = Ogre::Math::Cos(sunPitch);
	Ogre::Real sin_pitch = Ogre::Math::Sin(sunPitch);
	Ogre::Real cos_yaw   = Ogre::Math::Cos(sunRotation);
	Ogre::Real sin_yaw   = Ogre::Math::Sin(sunRotation);
	Ogre::Matrix3 Rz(cos_pitch, -sin_pitch, 0, 
		             sin_pitch,  cos_pitch, 0, 
					         0,          0, 1);
	Ogre::Matrix3 Ry( cos_yaw, 0, sin_yaw, 
		                    0, 1,       0, 
					 -sin_yaw, 0, cos_yaw);
	Ogre::Vector3 sunDirection = Ry * Rz * Ogre::Vector3(-1, 0, 0);
	sunDirection.normalise();
	
    // Set the ambient light.
    GameCore::mSceneMgr->setAmbientLight(sunAmbienceColour);
    
    // Add a directional light (for the sun).
	Ogre::Light* directionalLight;
    directionalLight = GameCore::mSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(sunBrightnessColour);
    directionalLight->setSpecularColour(sunSpecularColour);
	directionalLight->setDirection(sunDirection);
	
    // Create the skybox
	GameCore::mSceneMgr->setSkyBox(true, skyBoxMap);
}


/// @brief  Builds the initial arena.
void GraphicsApplication::setupArena (void)
{
    // Load and meshes and create entities
    Ogre::Entity* arenaEntity = GameCore::mSceneMgr->createEntity("Arena", "arena.mesh");
    arenaEntity->setMaterialName("arena_uv");
    arenaEntity->setCastShadows(true); // without shadows you can't see the seating rows
    
    Ogre::SceneNode* arenaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("ArenaNode", Ogre::Vector3(0, 0, 0));
    arenaNode->attachObject(arenaEntity);
	GameCore::mPhysicsCore->auto_scale_scenenode(arenaNode);
    arenaNode->setDebugDisplayEnabled( false );


    // ground plane, visible on the top down view only (unless something bad happens!!)
    /*Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane("GroundPlaneMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 5000, 20, 20, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* groundEntity = GameCore::mSceneMgr->createEntity("Ground", "GroundPlaneMesh");
    groundEntity->setMaterialName("Examples/GrassFloor");
    groundEntity->setCastShadows(false);
    
    // Create ground plane at -5 (below lowest arena point) to avoid z-index flickering/showing the plane through arena floor.
    Ogre::SceneNode* groundNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode", Ogre::Vector3(0, -5, 0));
    groundNode->attachObject(groundEntity);*/

	//Pass the arena info the the bigscreen so it can do calcualtions to draw
	//2D MAP VIEW
	Ogre::Vector3 arenaSize = arenaEntity->getBoundingBox().getSize();
	arenaSize = arenaSize*arenaNode->getScale();//Scale the size
	Ogre::Vector3 arenaLocation = arenaNode->getPosition();
	
	GameCore::mGraphicsCore->bigScreen->setMapCorner(arenaLocation);
	GameCore::mGraphicsCore->bigScreen->setMapSize(arenaSize);


    // create collideable floor so shit doesn't freefall. It will hit the floor.
    GameCore::mPhysicsCore->createFloorPlane( arenaNode );
}


/// @brief  Passes the frame listener down to the GraphicsCore.
void GraphicsApplication::createFrameListener (void)
{
    GraphicsCore::createFrameListener();
    
    // The gamecore stuff should really be handled in a different manner - I will come back and change this later.
	GameCore::mNetworkCore->init(NULL);

	// Handle Game play (this isn't the place to do this, this will be moved).
	GameCore::mGameplay = new Gameplay();
	Team* t1 = GameCore::mGameplay->createTeam("Team1Name");
	Team* t2 = GameCore::mGameplay->createTeam("Team2Name");
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
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

		if( mUserInput.isToggleConsole() )
			GameCore::mGui->toggleConsole();
		else if( mUserInput.isToggleChatbox() )
			GameCore::mGui->toggleChatbox();
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
	GameCore::mPlayerPool->getLocalPlayer()->processControlsFrameEvent(inputSnapshot, evt.timeSinceLastFrame, (1.0f / 60.0f));
    //mPlayerPool->getLocalPlayer()->updateCameraFrameEvent(mUserInput.getMouseXRel(), mUserInput.getMouseYRel());

    GameCore::mPowerupPool->frameEvent( evt );

    GameCore::mAudioCore->frameEvent(200);

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
