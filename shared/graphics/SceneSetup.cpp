/**
 * @file	SceneSetup.cpp
 * @brief 	Sets up the scene
 */

#include "stdafx.h"
#include "SharedIncludes.h"

// The shadowing method to use (1 = Stencils, 2 = Texturing, 3 = DSM, 4 = PSSM).
#define SHADOW_METHOD 2


SceneSetup::SceneSetup (void) :
    gfxSettingHDR(1.0f),
    gfxSettingBloom(1.0f),
    gfxSettingRadialBlur(1.0f),
    gfxSettingMotionBlur(1.0f)
{
}


SceneSetup::~SceneSetup (void)
{
    
}





/***************************************************************************
 *****************          SCENE SETUP FUNCTIONS          *****************
 ***************************************************************************/


/// @brief  Builds the compositor chain which adds post filters to the rendered image before being displayed.
void SceneSetup::setupCompositorChain (Ogre::Viewport* vp)
{
	// Collect required information.
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	
	// Create compositors. Where possible these are coded in the Examples.compositor script, but some need
	// access to certain features not available when this script is compiled.
	createMotionBlurCompositor();

	// Register the compositors.
	// This is done by first setting up the logic module for them and adding this as a listener so it
	// fires every time the compositor completes a pass allowing injection of values into the GPU
	// shaders which render the materials each pass, thus altering the behaviour of the compositor.
	// Finally add the compositors to the compositor chain and configure, then enable them.
	mHDRLogic = new HDRLogic;
	cm.registerCompositorLogic("HDR", mHDRLogic);
	//cm.addCompositor(vp, "HDR", 0);		// HDR must be at the front of the chain.
	//hdrLoader(vp, 0);

	mBloomLogic = new BloomLogic;
	cm.registerCompositorLogic("Bloom", mBloomLogic);
	cm.addCompositor(vp, "Bloom");
	loadBloom(vp, 0, 0.15f, 1.0f);

	mMotionBlurLogic = new MotionBlurLogic;
	cm.registerCompositorLogic("MotionBlur", mMotionBlurLogic);
	cm.addCompositor(vp, "MotionBlur");
	loadMotionBlur(vp, 0, 0.10f);

	mRadialBlurLogic = new RadialBlurLogic;
	cm.registerCompositorLogic("RadialBlur", mRadialBlurLogic);
	cm.addCompositor(vp, "RadialBlur");
	// radial blur has no loader as it is controlled by the players speed (Car.cpp).
}


/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadHDR (Ogre::Viewport* vp, uint8_t mode)
{
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();

	if (mode > 0)
		cm.setCompositorEnabled(vp, "HDR", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "HDR", true);
}

/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight)
{
	// reload bloom
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	
	// Scale the bloom values by the bloom graphical setting. This defaults to 1.
	blurWeight     *= gfxSettingBloom;
	originalWeight *= gfxSettingBloom;

	if (blurWeight > 0.0f)
		mBloomLogic->setBlurWeight(blurWeight);
	if (originalWeight > 0.0f)
		mBloomLogic->setOriginalWeight(originalWeight);
	if (mode > 0)
		cm.setCompositorEnabled(vp, "Bloom", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "Bloom", true);
}

/// @param mode	 The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur)
{
	// reload bloom
	Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
	
	// Scale the blur amount by the blur graphical setting. This defaults to 1.
	blur *= gfxSettingMotionBlur;

	if (blur > 0.0f)
		mMotionBlurLogic->setBlurStrength(blur);
	if (mode > 0)
		cm.setCompositorEnabled(vp, "MotionBlur", false);
	if (mode < 2)
		cm.setCompositorEnabled(vp, "MotionBlur", true);
}

void SceneSetup::setRadialBlur (Ogre::Viewport* vp, float blur)
{
	static bool enabled = false;

	// Scale the blur amount by the blur graphical setting. This defaults to 1.
	blur *= gfxSettingRadialBlur;

	if (enabled)
	{
		if (blur < 0.001f)
		{
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "RadialBlur", false);
			enabled = false;
		}
		else
		{
			mRadialBlurLogic->setBlurStrength(blur);
		}
	}
	else
	{
		if (blur > 0.001f)
		{
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "RadialBlur", true);
			mRadialBlurLogic->setBlurStrength(blur);
			enabled = true;
		}
	}
}


/// @brief Configure the shadow system. This should be the *FIRST* thing in the scene setup, because the shadow technique can alter the way meshes are loaded.
void SceneSetup::setupShadowSystem (void)
{
#if SHADOW_METHOD == 1
    /**** Stencil shadowing. No shaders necessary. ****/
    GameCore::mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    GameCore::mSceneMgr->setShadowFarDistance(150);
    GameCore::mSceneMgr->setShadowDirectionalLightExtrusionDistance(1000);
#elif SHADOW_METHOD == 2
    /**** Texture shadowmapping. Uses LiSPSM projection to enhance quality. ****/
    GameCore::mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
    GameCore::mSceneMgr->setShadowFarDistance(150);
    GameCore::mSceneMgr->setShadowDirectionalLightExtrusionDistance(1000);

    GameCore::mSceneMgr->setShadowDirLightTextureOffset(0.9f);
    GameCore::mSceneMgr->setShadowTextureCount(1);
    GameCore::mSceneMgr->setShadowTextureSize(2048);

    Ogre::LiSPSMShadowCameraSetup* shadowSetup = new Ogre::LiSPSMShadowCameraSetup();
    shadowSetup->setOptimalAdjustFactor(1);
    GameCore::mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(shadowSetup));
#elif SHADOW_METHOD == 3
    /**** Depth Shadowmapping with PCF filtering and LiSPSM projection. Ensure materials are correctly setup. ****/
    GameCore::mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);

    GameCore::mSceneMgr->setShadowFarDistance(150);
    GameCore::mSceneMgr->setShadowDirectionalLightExtrusionDistance(1000);
    GameCore::mSceneMgr->setShadowDirLightTextureOffset(0.9f);
    GameCore::mSceneMgr->setShadowTextureCount(1);
    GameCore::mSceneMgr->setShadowTextureConfig(0, 4096, 4096, Ogre::PF_FLOAT32_R);
    
    GameCore::mSceneMgr->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");          // Set the caster material.
    GameCore::mSceneMgr->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float/PCF");  // Set the receiver material (remove /PCF to disable).
    GameCore::mSceneMgr->setShadowTextureSelfShadow(true);
    GameCore::mSceneMgr->setShadowCasterRenderBackFaces(true);

    Ogre::LiSPSMShadowCameraSetup* shadowSetup = new Ogre::LiSPSMShadowCameraSetup();
    shadowSetup->setOptimalAdjustFactor(1);
    GameCore::mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(shadowSetup));
#elif SHADOW_METHOD == 4
    /**** Depth Shadowmapping with PSSM projection and LiSPSM filtering and split resolution. Ensure materials are correctly setup. ****/
    GameCore::mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    GameCore::mSceneMgr->setShadowFarDistance(150);
    GameCore::mSceneMgr->setShadowDirectionalLightExtrusionDistance(500);
    GameCore::mSceneMgr->setShadowDirLightTextureOffset(0.7f);
    GameCore::mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    GameCore::mSceneMgr->setShadowTextureCount(3);
    GameCore::mSceneMgr->setShadowTextureConfig(0, 1024, 1024, Ogre::PF_FLOAT32_RGB);
    GameCore::mSceneMgr->setShadowTextureConfig(1, 1024, 1024, Ogre::PF_FLOAT32_RGB);
    GameCore::mSceneMgr->setShadowTextureConfig(2, 512, 512, Ogre::PF_FLOAT32_RGB);
    GameCore::mSceneMgr->setShadowTextureSelfShadow(true);
    GameCore::mSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");
    GameCore::mSceneMgr->setShadowCasterRenderBackFaces(false);
    GameCore::mSceneMgr->setShadowColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
    
    Ogre::PSSMShadowCameraSetup* shadowSetup = new Ogre::PSSMShadowCameraSetup();
    shadowSetup->calculateSplitPoints(3, mCamera->getNearClipDistance() * 0.5, mCamera->getFarClipDistance());
    char str[64];
    sprintf(str, "\nnearClip=%.1f, farClip=%.1f\n\n", mCamera->getNearClipDistance(), mCamera->getFarClipDistance());
    OutputDebugString(str);
    shadowSetup->setSplitPadding(1);
    shadowSetup->setOptimalAdjustFactor(0, 5);
    shadowSetup->setOptimalAdjustFactor(1, 1);
    shadowSetup->setOptimalAdjustFactor(2, 0);
    GameCore::mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(shadowSetup));
#endif
}


/// @brief Configures the lighting for the scene, initialising all the lights.
void SceneSetup::setupLightSystem (void)
{
	// initialise the sun
    mWorldSun = GameCore::mSceneMgr->createLight("directionalLight");
    mWorldSun->setType(Ogre::Light::LT_DIRECTIONAL);

	// initialise (but don't attach) the weather system
	mWeatherSystem = GameCore::mSceneMgr->createParticleSystem("WeatherSystem", "Examples/RainSmall");

	// setup the lighting and weather system
	setWeather(0);
}


/// @brief  Sets the weather and lighting mode for the scene. Without setting this no lights will be turned on.
/// @param  mode	The lighting mode to use. 0 = Morning, 1 = Noon, 2 = Stormy.
void SceneSetup::setWeather (uint8_t mode)
{
	static bool weatherSystemAttached = false;

	Ogre::Degree sunRotation; // rotation horizontally (yaw) from +x axis
	Ogre::Degree sunPitch;	  // rotation downwards (pitch) from horizontal
	float sunBrightness[4];   // RGBA
	float   sunSpecular[4];   // RGBA
	float   sunAmbience[4];   // RGBA
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
		sunBrightness[3] = 1000;
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
			GameCore::mSceneMgr->getRootSceneNode()->attachObject(mWeatherSystem);
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
	Ogre::Matrix3 Rz(cos_pitch, -sin_pitch,       0, 
		             sin_pitch,  cos_pitch,       0, 
					         0,          0,       1);
	Ogre::Matrix3 Ry(  cos_yaw,          0, sin_yaw, 
		                     0,          1,       0, 
					  -sin_yaw,          0, cos_yaw);
	Ogre::Vector3 sunDirection = Ry * Rz * Ogre::Vector3(-1, 0, 0);
	sunDirection.normalise();
	
    // Set the ambient light.
	GameCore::mSceneMgr->setAmbientLight(sunAmbienceColour);
    
    // Add a directional light (for the sun).
    mWorldSun->setDiffuseColour(sunBrightnessColour);
    mWorldSun->setSpecularColour(sunSpecularColour);
	mWorldSun->setDirection(sunDirection);
    mWorldSun->setCastShadows(true);
	
    // Create the skybox
	GameCore::mSceneMgr->setSkyBox(true, skyBoxMap, 1000);
}


/// @brief Configures the particle system.
void SceneSetup::setupParticleSystem (void)
{
	// Set nonvisible timeout
	Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);
    
    // Create spark system
	//mSparkSystem = GameCore::mSceneMgr->createParticleSystem("SparkSystem", "CollisionDomain/Spark");
    //GameCore::mSceneMgr->getRootSceneNode()->attachObject(mSparkSystem);
}


void SceneSetup::setupArena (void)
{
    // Load and meshes and create entities
    Ogre::Entity* arenaEntity = GameCore::mSceneMgr->createEntity("Arena", "arena.mesh");
#if SHADOW_METHOD == 2
    arenaEntity->setCastShadows(false);
#else
    arenaEntity->setCastShadows(true);
#endif
    Ogre::SceneNode* arenaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("ArenaNode");
    arenaNode->attachObject(arenaEntity);
	GameCore::mPhysicsCore->auto_scale_scenenode(arenaNode);
    GameCore::mPhysicsCore->attachArenaCollisionMesh(arenaNode);
    
    // Load the extra props (ninja + tree) into the scene.
    Ogre::Entity* ninjaEntity = GameCore::mSceneMgr->createEntity("Ninja", "ninja.mesh");
    Ogre::Entity* treeEntity  = GameCore::mSceneMgr->createEntity("Tree",  "palm_tree1.mesh");    // alternatively use basic_tree.mesh
    ninjaEntity->setCastShadows(true);
#if SHADOW_METHOD == 1
    treeEntity->setCastShadows(false);
#else
    treeEntity->setCastShadows(true);
#endif
    Ogre::SceneNode* ninjaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    Ogre::SceneNode* treeNode  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("TreeNode");
    ninjaNode->attachObject(ninjaEntity);
    treeNode->attachObject(treeEntity);
	GameCore::mPhysicsCore->auto_scale_scenenode(ninjaNode);
	GameCore::mPhysicsCore->auto_scale_scenenode(treeNode);
    ninjaNode->translate(50.0f, -10.5f,  3.0f);
    treeNode->translate( 50.0f, -10.5f, -3.0f);
}


void SceneSetup::setupGUI (void)
{
    // Attach and start the GUI renderer.
    mGuiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    GameCore::mGui->initialiseGUI();
}


void SceneSetup::createMotionBlurCompositor (void)
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