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
	gfxSettingHDR        = 1.0f;
	gfxSettingBloom      = 1.0f;
	gfxSettingRadialBlur = 1.0f;
	gfxSettingMotionBlur = 1.0f;
}


/// @brief  Destructor.
GraphicsApplication::~GraphicsApplication (void)
{
}


/// @brief  Creates the initial scene prior to the first render pass, adding objects etc.
void GraphicsApplication::createScene (void)
{
	// Save reference
	GameCore::mGraphicsApplication = this;

	// Setup the GUI
	mGuiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	GameCore::mGui->setupGUI();

	// Setup the scene
	setupShadowSystem();
	setupCompositorChain();
    setupLighting();
    setupArena();
    setupNetworking();

    // Load the ninjas into the scene. This is for testing purposes only and can be removed later.
    Ogre::Entity* ninjaEntity = GameCore::mSceneMgr->createEntity("Ninja", "ninja.mesh");
    ninjaEntity->setCastShadows(true);
    Ogre::SceneNode* ninjaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    ninjaNode->attachObject(ninjaEntity);
    ninjaNode->scale(0.2f, 0.2f, 0.2f);
    ninjaNode->translate(50.0f, -3.0f, 0);

	// Attach the GUI components
	GameCore::mGui->displayConnectBox();
	GameCore::mGui->displayConsole();
	GameCore::mGui->displayChatbox();

	createSpeedo();
}

/// @brief Draws the speedo on-screen
void GraphicsApplication::createSpeedo (void)
{
	// Create our speedometer overlays
	Ogre::Overlay *olSpeedo = Ogre::OverlayManager::getSingleton().create( "OVERLAY_SPD" );
	olSpeedo->setZOrder( 500 );
	olSpeedo->show();

	olcSpeedo = static_cast<Ogre::OverlayContainer*> ( Ogre::OverlayManager::getSingleton().
		createOverlayElement( "Panel", "SPEEDO" ) );

	olcSpeedo->setMetricsMode( Ogre::GMM_PIXELS );
	olcSpeedo->setHorizontalAlignment( Ogre::GHA_LEFT );
	olcSpeedo->setVerticalAlignment( Ogre::GVA_BOTTOM );
	olcSpeedo->setDimensions( 250, 250 );
	olcSpeedo->setMaterialName( "speedo_main" );
	olcSpeedo->setPosition( 20, -270 );

	olSpeedo->add2D( olcSpeedo );

	oleNeedle = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "SPEEDONEEDLE" );
	oleNeedle->setMetricsMode( Ogre::GMM_PIXELS );
	oleNeedle->setDimensions( 250, 250 );
	oleNeedle->setMaterialName( "speedo_needle" );

	olcSpeedo->addChild( oleNeedle );

    createGearDisplay();

	updateSpeedo( 0, 0 );
}

/// @brief Draws the gear display
void GraphicsApplication::createGearDisplay (void)
{
    // Create our speedometer overlays
	/*Ogre::Overlay *olGear = Ogre::OverlayManager::getSingleton().create( "OVERLAY_GEAR" );
	olGear->setZOrder( 500 );
	olGear->show();*/

	oleGear = Ogre::OverlayManager::getSingleton().createOverlayElement( "Panel", "GEAR" );

	oleGear->setMetricsMode( Ogre::GMM_PIXELS );
	oleGear->setHorizontalAlignment( Ogre::GHA_LEFT );
	oleGear->setVerticalAlignment( Ogre::GVA_BOTTOM );
	oleGear->setDimensions( 32, 57 );
	oleGear->setMaterialName( "gearoff" );
	oleGear->setPosition( 109, -73 );

	olcSpeedo->addChild( oleGear );

	updateSpeedo( 0, -1 );
}

/// @brief Configure the shadow system. This should be the *FIRST* thing in the scene setup, because the shadow technique can alter the way meshes are loaded.
void GraphicsApplication::setupShadowSystem (void)
{
    // Set the shadow renderer
    GameCore::mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	// THIS NEEDS TO BE SET, DEFAULTS TO ZERO. GameCore::mSceneMgr->setShadowFarDistance();
}


/// @brief Configures the lighting for the scene, initialising all the lights.
void GraphicsApplication::setupLighting (void)
{
	// initialise the sun
    worldSun = GameCore::mSceneMgr->createLight("directionalLight");
    worldSun->setType(Ogre::Light::LT_DIRECTIONAL);

	// initialise (but don't attach) the weather system
	weatherSystem = GameCore::mSceneMgr->createParticleSystem("WeatherSystem", "Examples/RainSmall");
	weatherSystemAttached = false;

	// setup the lighting and weather system
	setWeatherMode(1);
}


/// @brief Builds the compositor chain which adds post filters to the rendered image before being displayed.
void GraphicsApplication::setupCompositorChain (void)
{
	// Collect required information.
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	Ogre::Viewport* vp = mCamera->getViewport();
	
	// Create compositors. Where possible these are coded in the Examples.compositor script, but some need
	// access to certain features not available when this script is compiled.
	createMotionBlurCompositor();

	// Rehister the compositors.
	// This is done by first setting up the logic module for them and adding this as a listener so it
	// fires every time the compositor completes a pass allowing injection of values into the GPU
	// shaders which render the materials each pass, thus altering the behaviour of the compositor.
	hdrLogic        = new HDRLogic;
	bloomLogic      = new BloomLogic;
	radialBlurLogic = new RadialBlurLogic;
	motionBlurLogic = new MotionBlurLogic;
	cm.registerCompositorLogic("HDR",        hdrLogic);
	cm.registerCompositorLogic("Bloom",      bloomLogic);
	cm.registerCompositorLogic("RadialBlur", radialBlurLogic);
	cm.registerCompositorLogic("MotionBlur", motionBlurLogic);

	// Add the compositors to the compositor chain.
	cm.addCompositor(vp, "HDR", 0);		// HDR must be at the front of the chain.
	cm.addCompositor(vp, "Bloom");
	cm.addCompositor(vp, "MotionBlur");
	cm.addCompositor(vp, "RadialBlur");
	
	// Enable and configure compositors (radial blur is controlled by the players speed).
	hdrLoader(0);
	bloomLoader(0, 0.15f, 1.0f);
	motionBlurLoader(0, 0.10f);
}

/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void GraphicsApplication::hdrLoader (uint8_t mode)
{
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	Ogre::Viewport* vp = mCamera->getViewport();

	if (mode > 0)
		cm.setCompositorEnabled(vp, "HDR", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "HDR", true);
}

/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void GraphicsApplication::bloomLoader (uint8_t mode, float blurWeight, float originalWeight)
{
	// reload bloom
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	Ogre::Viewport* vp = mCamera->getViewport();

	if (blurWeight > 0.0f)
		bloomLogic->setBlurWeight(blurWeight);
	if (originalWeight > 0.0f)
		bloomLogic->setOriginalWeight(originalWeight);
	if (mode > 0)
		cm.setCompositorEnabled(vp, "Bloom", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "Bloom", true);
}

/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void GraphicsApplication::motionBlurLoader (uint8_t mode, float blur)
{
	// reload bloom
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	Ogre::Viewport* vp = mCamera->getViewport();

	if (blur > 0.0f)
		motionBlurLogic->setBlurStrength(blur);
	if (mode > 0)
		cm.setCompositorEnabled(vp, "MotionBlur", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "MotionBlur", true);
}

void GraphicsApplication::setRadialBlur (float blur)
{
	static bool enabled = false;

	// Scale the blur amount by the blur graphical setting. This defaults to 1.
	blur *= gfxSettingRadialBlur;

	if (enabled)
	{
		if (blur < 0.001f)
		{
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "RadialBlur", false);
			enabled = false;
		}
		else
		{
			radialBlurLogic->setBlurStrength(blur);
		}
	}
	else
	{
		if (blur > 0.001f)
		{
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "RadialBlur", true);
			radialBlurLogic->setBlurStrength(blur);
			enabled = true;
		}
	}
}


/// @brief  Sets the weather and lighting mode for the scene. Without setting this no lights will be turned on.
/// @param  mode	The lighting mode to use. 0 = Morning, 1 = Noon, 2 = Stormy.
void GraphicsApplication::setWeatherMode (uint8_t mode)
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
		
		if (weatherSystemAttached)
			GameCore::mSceneMgr->getRootSceneNode()->detachObject("WeatherSystem");
		weatherSystemAttached = false;
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
		
		if (weatherSystemAttached)
			GameCore::mSceneMgr->getRootSceneNode()->detachObject("WeatherSystem");
		weatherSystemAttached = false;
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
		
		if (!weatherSystemAttached)
			GameCore::mSceneMgr->getRootSceneNode()->attachObject(weatherSystem);
		weatherSystemAttached = true;
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
    worldSun->setDiffuseColour(sunBrightnessColour);
    worldSun->setSpecularColour(sunSpecularColour);
	worldSun->setDirection(sunDirection);
	
    // Create the skybox
	GameCore::mSceneMgr->setSkyBox(true, skyBoxMap);
}


/// @brief Configures the particle system.
void GraphicsApplication::setupParticles (void)
{
	// set nonvisible timeout
	Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);
}


/// @brief  Builds the initial arena.
void GraphicsApplication::setupArena (void)
{
    // Load and meshes and create entities
    Ogre::Entity* arenaEntity = GameCore::mSceneMgr->createEntity("Arena", "arena.mesh");
    arenaEntity->setMaterialName("arena_uv");
    arenaEntity->setCastShadows(true);
    
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



    // create collideable floor so shit doesn't freefall. It will hit the floor.
    GameCore::mPhysicsCore->createFloorPlane( arenaNode );
    GameCore::mPhysicsCore->createWallPlanes();
}


/// @brief  Configures the networking, retreiving the required data from the server.
void GraphicsApplication::setupNetworking (void)
{
}


/// @brief  Passes the frame listener down to the GraphicsCore.
void GraphicsApplication::createFrameListener (void)
{
    firstFrameOccurred = false;
    //mClock = new btClock();
    GraphicsCore::createFrameListener();
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    // MUST BE THE FIRST THING - do the core things (GraphicsCore is extended by this class)
    if (!GraphicsCore::frameRenderingQueued(evt)) return false;

    // Toggle on screen widgets
    /*if (mUserInput.isToggleWidget()) 
    {
        mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
        mDetailsPanel->show();
    }*/
    
    // NOW WE WILL DO EVERYTHING BASED OFF THE LATEST KEYBOARD / MOUSE INPUT

	InputState *inputSnapshot = mUserInput.getInputState();

	if( mUserInput.isToggleConsole() )      GameCore::mGui->toggleConsole();
	else if( mUserInput.isToggleChatbox() ) GameCore::mGui->toggleChatbox();
    
    // Process the networking. Sends client's input and receives data
    GameCore::mNetworkCore->frameEvent(inputSnapshot);

	if( NetworkCore::bConnected == true )
	{
		// Process the player pool. Perform updates on other players
		GameCore::mPlayerPool->frameEvent();

		// Apply controls the player (who will be moved on frameEnd and frameStart).
		if( GameCore::mPlayerPool->getLocalPlayer()->getCar() != NULL )
		{
			GameCore::mPlayerPool->getLocalPlayer()->processControlsFrameEvent(inputSnapshot, evt.timeSinceLastFrame, (1.0f / 60.0f));
			GameCore::mPlayerPool->getLocalPlayer()->updateCameraFrameEvent(mUserInput.getMouseXRel(), mUserInput.getMouseYRel(), mUserInput.getMouseZRel());

            // update GUI
			float speedmph = GameCore::mPlayerPool->getLocalPlayer()->getCar()->getCarMph();
            int   curGear  = GameCore::mPlayerPool->getLocalPlayer()->getCar()->getGear();
			float avgfps   = mWindow->getAverageFPS();

			CEGUI::Window *mph = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/mph" );
			CEGUI::Window *fps = CEGUI::WindowManager::getSingleton().getWindow( "root_wnd/fps" );
			char szSpeed[64];
			char szFPS[64];
			sprintf( szSpeed, "MPH: %f", speedmph );
			sprintf( szFPS,   "FPS: %f", avgfps);
			mph->setText( szSpeed );
			fps->setText( szFPS );
			updateSpeedo( speedmph, curGear );

			// 
		}

	}

    /*  NOTE TO SELF (JAMIE)
        Client doesn't want to do PowerupPool::frameEvent() when powerups are networked
        All powerup removals handled by the server's collision events
        In fact, client probably shouldn't register any collision callbacks for powerups
    */
    GameCore::mPowerupPool->frameEvent( evt );

    // FUTURE
    // game will run x ticks behind the server
    // when a new snapshot is received, it should be in the client's future
    // interpolate based on snapshot timestamps

    // Cleanup frame specific objects so we don't rape memory. If you want to remember some, delete them later!
    delete inputSnapshot;

    // Minimum of 30 FPS (maxSubsteps=2) before physics becomes wrong
    GameCore::mPhysicsCore->stepSimulation(evt.timeSinceLastFrame, 2, (1.0f / 60.0f));

    return true;
}


/// @brief  Called once a frame every time processing for a frame has begun.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameStarted(const Ogre::FrameEvent& evt)
{
    return true;
}


/// @brief  Called once a frame every time processing for a frame has ended.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameEnded(const Ogre::FrameEvent& evt)
{
    return true;
}

/// @brief	Update the rotation of the speedo needle
/// @param	fSpeed	Float containing speed of car in mph
/// @param  iGear   Current car gear
void GraphicsApplication::updateSpeedo (float fSpeed, int iGear)
{
	if( fSpeed < 0 )
		fSpeed *= -1;

	if( fSpeed > 220 )
		fSpeed = 220;

	float iDegree = 58; // This is 0 for some reason

	// 1 mph = 298 / 220 degrees

	iDegree = 58 - ( fSpeed * ( 298.0f / 220.0f ) );

	Ogre::Material *matNeedle = oleNeedle->getMaterial().get();
	Ogre::TextureUnitState *texNeedle = 
		matNeedle->getTechnique( 0 )->getPass( 0 )->getTextureUnitState( 0 );
	texNeedle->setTextureRotate( Ogre::Degree( iDegree ) );

    if( iGear >= 0 )
    {
        char cnum[2];
        itoa( iGear, cnum, 10 );
        cnum[1] = '\0';

        char matname[32];
        sprintf( matname, "gear%s", cnum );
        oleGear->setMaterialName( matname );
    }
    else
        oleGear->setMaterialName( "gearoff" );
}


void GraphicsApplication::createMotionBlurCompositor()
{
	// Motion blur effect
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
			"MotionBlur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		t->setCompositorLogicName("MotionBlur");
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		// Render scene texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		// Do the motion blur, accepting listener injection
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			pass->setIdentifier(700);
			}
		}
		// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
}


// Main function. Bootstraps the application.
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
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
