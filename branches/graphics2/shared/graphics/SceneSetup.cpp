/**
 * @file    SceneSetup.cpp
 * @brief     Sets up the scene
 */

#include "stdafx.h"
#include "SharedIncludes.h"

// The shadowing method to use (1 = Stencils, 2 = Texturing, 3 = DSM, 4 = PSSM).
#define SHADOW_METHOD 2
#define ARENA 1


SceneSetup::SceneSetup (void) :
    mGfxSettingHDR(1.0f),
    mGfxSettingBloom(1.0f),
    mGfxSettingRadialBlur(1.0f),
    mGfxSettingMotionBlur(1.0f)
{
}


SceneSetup::~SceneSetup (void)
{
    
}





/***************************************************************************
 *****************          SCENE SETUP FUNCTIONS          *****************
 ***************************************************************************/


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
/// @param  mode    The lighting mode to use. 0 = Morning, 1 = Noon, 2 = Stormy.
void SceneSetup::setWeather (uint8_t mode)
{
    static bool weatherSystemAttached = false;

    Ogre::Degree sunRotation; // rotation horizontally (yaw) from +x axis
    Ogre::Degree sunPitch;      // rotation downwards (pitch) from horizontal
    float sunBrightness[4];   // RGBA
    float   sunSpecular[4];   // RGBA
    float   sunAmbience[4];   // RGBA
    std::string skyBoxMap;
    float sf; // scaling factor

    // Set lighting constants
    if (mode < 1)    // Morning
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
    // Build the emitter definitions. This has to be done for all particle systems which need to
    // place their own emitters rather than having a fixed number of emitters 'hardcoded' in the
    // script.
    // Spark parameters
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("angle",          "5") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("emission_rate",  "200") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("velocity_min",   "35") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("velocity_max",   "40") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live",   "0.5") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("colour_range_start", "1.000 1.000 0.847 1.0") );
    mSparkParams.insert( std::pair<Ogre::String, Ogre::String>("colour_range_end",   "0.851 0.737 0.565 1.0") );

    // Explosion nucleus parameters
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("direction",      "0.0 0.0 0.0") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("angle",          "0") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("emission_rate",  "20") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("velocity",       "0") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live",   "0.5") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("duration",       "0.5") );
    //mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("repeat_delay",   "1.5") );
    mExplosionNucleusParams.insert( std::pair<Ogre::String, Ogre::String>("colour",         "0.871 0.392 0.067 1") );

    // Explosion smoke parameters
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("direction",      "0.0 1.0 0.0") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("angle",          "0") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("emission_rate",  "20") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("velocity",       "1") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live_min", "1.5") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live_max", "2.0") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("duration",       "0.5") );
    //mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("repeat_delay",   "1.5") );
    mExplosionSmokeParams.insert( std::pair<Ogre::String, Ogre::String>("colour",         "0.1 0.1 0.1 1") );

    // Explosion debris parameters
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("angle",          "3") );
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("emission_rate",  "20") );
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live_min", "1.0") );
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("time_to_live_max", "1.5") );
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("duration",       "5.0") );
    mExplosionDebrisParams.insert( std::pair<Ogre::String, Ogre::String>("colour",         "0.9 0.8 0.8 1") );

    
    // Setup the particle systems.
    // Set nonvisible timeout.
    Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);
    
    // Create systems.
    mSparkSystem            = GameCore::mSceneMgr->createParticleSystem("SparkSystem",            "CollisionDomain/Spark");
    mExplosionNucleusSystem = GameCore::mSceneMgr->createParticleSystem("ExplosionNucleusSystem", "CollisionDomain/Explosion/Nucleus");
    mExplosionSmokeSystem   = GameCore::mSceneMgr->createParticleSystem("ExplosionSmokeSystem",   "CollisionDomain/Explosion/Smoke");
    mExplosionDebrisSystem  = GameCore::mSceneMgr->createParticleSystem("ExplosionDebrisSystem",  "CollisionDomain/Explosion/Debris");
    GameCore::mSceneMgr->getRootSceneNode()->attachObject(mSparkSystem);
    GameCore::mSceneMgr->getRootSceneNode()->attachObject(mExplosionNucleusSystem);
    GameCore::mSceneMgr->getRootSceneNode()->attachObject(mExplosionSmokeSystem);
    GameCore::mSceneMgr->getRootSceneNode()->attachObject(mExplosionDebrisSystem);
    
    // Add the VIP Nodes and systems
    // This is a completely inappropriate place to do this and I would normally freak out if I saw this, but I'm waiting for a better 
    // place (in the team class for example) to be made available.
    mVIPIcon[0]  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("VIPNode1");
    mVIPIcon[1]  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("VIPNode2");
    Ogre::ParticleSystem* crownEffect1 = GameCore::mSceneMgr->createParticleSystem("crownSystem1", "CollisionDomain/Streak");
    Ogre::ParticleSystem* crownEffect2 = GameCore::mSceneMgr->createParticleSystem("crownSystem2", "CollisionDomain/Streak");
    mVIPIcon[0]->attachObject(crownEffect1);
    mVIPIcon[1]->attachObject(crownEffect2);
    Ogre::SceneNode* VIPMesh1 = mVIPIcon[0]->createChildSceneNode("VIPMeshNode1");
    Ogre::SceneNode* VIPMesh2 = mVIPIcon[1]->createChildSceneNode("VIPMeshNode2");
    Ogre::Entity* crownEntity1  = GameCore::mSceneMgr->createEntity("CrownEntity1",  "crown.mesh");
    Ogre::Entity* crownEntity2  = GameCore::mSceneMgr->createEntity("CrownEntity2",  "crown.mesh");
    GameCore::mPhysicsCore->auto_scale_scenenode(VIPMesh1);
    GameCore::mPhysicsCore->auto_scale_scenenode(VIPMesh2);
    VIPMesh1->scale(1.5f, 0.8f, 0.8f);
    VIPMesh2->scale(1.5f, 0.8f, 0.8f);
    VIPMesh1->attachObject(crownEntity1);
    VIPMesh2->attachObject(crownEntity2);
}


void SceneSetup::setupArenaNodes (void)
{
    // Create the node
    Ogre::SceneNode* arenaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("ArenaNode");
    
    // Scale the node
#if ARENA == 1
    GameCore::mPhysicsCore->auto_scale_scenenode(arenaNode);
#else
    //GameCore::mPhysicsCore->auto_scale_scenenode(arenaNode);
    float cpScale = 0.25f;
    arenaNode->scale(cpScale, cpScale, cpScale);
#endif
}

void SceneSetup::setupArenaGraphics (void)
{
    // Load the arena node and create the graphics only nodes (nodes also required for physics should be created
    // in setupArenaNodes) and scale them (loaded nodes have already been scaled).
    Ogre::SceneNode* arenaNode = GameCore::mSceneMgr->getSceneNode("ArenaNode");
    Ogre::SceneNode* ninjaNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    Ogre::SceneNode* treeNode  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("TreeNode");
    GameCore::mPhysicsCore->auto_scale_scenenode(ninjaNode);
    GameCore::mPhysicsCore->auto_scale_scenenode(treeNode);

    // Load the arena mesh.
#if ARENA == 1
    Ogre::Entity* arenaEntity = GameCore::mSceneMgr->createEntity("Arena", "arena.mesh");
#else
    Ogre::Entity* arenaEntity = GameCore::mSceneMgr->createEntity("Arena", "carpark.mesh");
#endif
#if SHADOW_METHOD == 2
    arenaEntity->setCastShadows(false);
#else
    arenaEntity->setCastShadows(true);
#endif

    // Load the graphics only props (ninja + tree) into the scene.
    Ogre::Entity* ninjaEntity = GameCore::mSceneMgr->createEntity("Ninja", "ninja.mesh");
    Ogre::Entity* treeEntity  = GameCore::mSceneMgr->createEntity("Tree",  "palm_tree1.mesh");  // alternatively use basic_tree.mesh
    ninjaEntity->setCastShadows(true);
#if SHADOW_METHOD == 1
    treeEntity->setCastShadows(false);
#else
    treeEntity->setCastShadows(true);
#endif

    // Attach the entities to their respective nodes.
    arenaNode->attachObject(arenaEntity);
    ninjaNode->attachObject(ninjaEntity);
    treeNode->attachObject(treeEntity);
    ninjaNode->translate(50.0f, -10.5f,  3.0f);
    treeNode->translate( 50.0f, -10.5f, -3.0f);
}

void SceneSetup::setupArenaPhysics (void)
{
    // Load the arena node
    Ogre::SceneNode* arenaNode = GameCore::mSceneMgr->getSceneNode("ArenaNode");

    // Construct the collision meshes
#if ARENA == 1
    GameCore::mPhysicsCore->attachCollisionMesh(arenaNode, "arena_collision.mesh", MESH_SCALING_CONSTANT);
#else
    GameCore::mPhysicsCore->attachCollisionMesh(arenaNode, "carpark_collision.mesh", cpScale);
#endif
}

void SceneSetup::setupMeshDeformer (void)
{
    smallCarBodyMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_SmallCarBody",     "small_car_body.mesh");
    smallCarLDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_SmallCarLDoor",    "small_car_ldoor.mesh");
    smallCarRDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_SmallCarRDoor",    "small_car_rdoor.mesh");
    smallCarFBumperMesh = GameCore::mSceneMgr->createEntity("UnIqUe_SmallCarFBumper",  "small_car_fbumper.mesh");
    smallCarRBumperMesh = GameCore::mSceneMgr->createEntity("UnIqUe_SmallCarRBumper",  "small_car_rbumper.mesh");

    bangerBodyMesh        = GameCore::mSceneMgr->createEntity("UnIqUe_BangerBody",       "banger_body.mesh");
    bangerFLDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerLDoor",      "banger_fldoor.mesh");
    bangerFRDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerRDoor",      "banger_frdoor.mesh");
    bangerRLDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerRLDoor",     "banger_rldoor.mesh");
    bangerRRDoorMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerRRDoor",     "banger_rrdoor.mesh");
    bangerFBumperMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerFBumper",    "banger_fbumper.mesh");
    bangerRBumperMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_BangerRBumper",    "banger_rbumper.mesh");

    truckBodyMesh       = GameCore::mSceneMgr->createEntity("UnIqUe_TruckBody",        "truck_body.mesh");
    truckLDoorMesh        = GameCore::mSceneMgr->createEntity("UnIqUe_TruckLDoor",       "truck_ldoor.mesh");
    truckRDoorMesh        = GameCore::mSceneMgr->createEntity("UnIqUe_TruckRDoor",       "truck_rdoor.mesh");
    truckRBumperMesh    = GameCore::mSceneMgr->createEntity("UnIqUe_TruckRBumper",     "truck_rbumper.mesh");
    
    meshDeformer = new MeshDeformer();
}


void SceneSetup::setupGUI (void)
{
    static bool guiSetup = false;
    if (!guiSetup)
    {
        // Attach and start the GUI renderer.
        OutputDebugString("Bootstrapping CEGUI\n");
        mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
        OutputDebugString("Initialising GUI\n");
        GameCore::mGui->initialiseGUI();
        OutputDebugString("GUI Initialised\n");
    }
}


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
    //cm.addCompositor(vp, "HDR", 0);        // HDR must be at the front of the chain.
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


/// @param mode     The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadHDR (Ogre::Viewport* vp, uint8_t mode)
{
    Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();

    if (mode > 0)
        cm.setCompositorEnabled(vp, "HDR", false);
    if (mode < 2)
        cm.setCompositorEnabled(vp, "HDR", true);
}

/// @param mode     The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight)
{
    // reload bloom
    Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
    
    // Scale the bloom values by the bloom graphical setting. This defaults to 1.
    blurWeight     *= mGfxSettingBloom;
    originalWeight *= mGfxSettingBloom;

    if (blurWeight > 0.0f)
        mBloomLogic->setBlurWeight(blurWeight);
    if (originalWeight > 0.0f)
        mBloomLogic->setOriginalWeight(originalWeight);
    if (mode > 0)
        cm.setCompositorEnabled(vp, "Bloom", false);
    if (mode < 2)
        cm.setCompositorEnabled(vp, "Bloom", true);
}

/// @param mode     The mode of operation for the function. 0 to load s the compositor, 1 to reload, 2 to unload.
void SceneSetup::loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur)
{
    // reload bloom
    Ogre::CompositorManager& cm = Ogre::CompositorManager::getSingleton();
    
    // Scale the blur amount by the blur graphical setting. This defaults to 1.
    blur *= mGfxSettingMotionBlur;

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
    blur *= mGfxSettingRadialBlur;

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