/**
 * @file    SceneSetup.h
 * @brief     Sets up the scene
 */
#ifndef SCENESETUP_H
#define SCENESETUP_H

// Includes
#include "stdafx.h"
#include "PostFilterLogic.h"
#include <stdint.h>



//------------------------------ DEFINITIONS ------------------------------//
// PARTICLE EFFECTS -------------------------------------------------------
// Global particle effects:
#define PARTICLE_EFFECT_VIPSTREAK   // Sparkly streaks emitted from VIP crowns.
#define PARTICLE_EFFECT_EXPLOSION   // Explosion on death.
//#define PARTICLE_EFFECT_SHRAPNEL    // Shrapnel from crunching collisions.
//#define PARTICLE_EFFECT_SHRAPNEL_SPARKS   // Point-based sparks emitted alongside shrapnel. Does nothing if shrapnel is not defined.
//#define PARTICLE_EFFECT_SPARKS      // Sparks emitted from scraping collisions.
#define PARTICLE_EFFECT_SPLINTERS   // Splinters thrown off when powerup crates are collected.
// Local particle effects:
#define PARTICLE_EFFECT_DUST        // Dust from skidding etc.
#define PARTICLE_EFFECT_EXHAUST     // Exhaust smoke from cars bums at engine idle/high work.
//#define PARTICLE_EFFECT_SMOKE       // Smoke emitted from damaged cars.
//#define PARTICLE_EFFECT_FIRE        // Flames emitted from dead cars.

// Enabled high quality (colliding) shrapnel effect. PARTICLE_EFFECT_SHRAPNEL must be defined for this to have any effect.
// A low quality, non-colliding version is used if this is not defined but shrapnel is enabled.
//#define PARTICLE_EFFECT_SHRAPNEL_HIQUAL


// COMPOSITORS ------------------------------------------------------------
// Note all of these have to be defined for benchmarking to work.
//#define COMPOSITOR_MOTION_BLUR    // Blur applied to fast moving objects on your screen.
#define COMPOSITOR_RADIAL_BLUR    // Blur applied to your screen at high speeds.
//#define COMPOSITOR_HDR            // Advanced lighting effects. Defining this loads but DOES NOT enable HDR. HDR is broken but this define is used as a workaround for the benchmarking, which won't run without it.
#define COMPOSITOR_BLOOM          // Blooms the lighting effects to give a chunky punch to the light set.


// SHADOW SYSTEM ----------------------------------------------------------
#define SHADOW_METHOD 2             // The shadowing method to use (1 = Stencils, 2 = Texturing, 3 = DSM, 4 = PSSM).
#define SHADOW_TEXTURE_SIZE 2048    // The texture size to render the shadows to. default 2048 (gives a 2048x2048 texture). Meaningless if SHADOW_METHOD != 2.
#define SHADOW_FAR_DISTANCE 150     // The maximum distance at which shadows will be visible. Further distances (>= 150 greatly reduce visible cutoff on our arenas), however shorter distances will improve the perceived quality of lower resolution textures.


// PROCEDURAL ARENA SYSTEM ------------------------------------------------
#define FOREST_ARENA_HQ_TREE_COUNT  50  // The number of high quality trees to use (187 triangles ea.) - have a wonky trunk.
#define FOREST_ARENA_LQ_TREE_COUNT 700  // The number of low  quality trees to use (125 triangles ea.) - have a straight trunk.
#define FOREST_ARENA_LQ_TREE_CUTOFF 10  // The furthest distance away from the fence that the high quality trees can spawn (in metres).


// TEXTURE SYSTEM ---------------------------------------------------------
#define TEXTURE_ENV_SWAPPING        // Allows the environment maps to be swapped in and out based on arena. Basically a work around for Zlatan, it shouldn't comprise a performance increase...


// MENU SYSTEM ------------------------------------------------------------
//#define MENU_LOBBY                  // Enables the lobby which is shown when first launching the game.




//------------------------------ CLASSES ------------------------------//
class MeshDeformer;
/**
 *  @brief     Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */

enum ArenaID
{
    COLOSSEUM_ARENA,
    FOREST_ARENA,
    QUARRY_ARENA,

    ARENA_COUNT,
};

class SceneSetup
{
public:
    SceneSetup (void);
    virtual ~SceneSetup (void);
    
#ifdef COMPOSITOR_HDR
    void loadHDR (Ogre::Viewport* vp, uint8_t mode);
#endif
#ifdef COMPOSITOR_BLOOM
    void loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight);
#endif
#ifdef COMPOSITOR_MOTION_BLUR
    void loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur);
#endif
#ifdef COMPOSITOR_RADIAL_BLUR
    void setRadialBlur (Ogre::Viewport* vp, float blur);
#endif

    void createArenaCollisionShapes();
    void loadArena (ArenaID aid);
    void unloadArena (ArenaID aid);

    // Graphical effect settings. Adjusts the scale of the effect - default is 1.
#ifdef COMPOSITOR_HDR
    float mGfxSettingHDR;         // Strength of the High Dynamic Range lighting.
#endif
#ifdef COMPOSITOR_BLOOM
    float mGfxSettingBloom;       // Strength of the bloom lighting artifact.
#endif
#ifdef COMPOSITOR_MOTION_BLUR
    float mGfxSettingMotionBlur;  // Amount of blurring of relatively moving objects.
#endif
#ifdef COMPOSITOR_RADIAL_BLUR
    float mGfxSettingRadialBlur;  // Amount of blurring encountered at high speeds.
#endif

    // These are the deformable meshes on each car, they need to be accessable to each car class so that the relevant meshes
    // can be cloned and added to the car
    Ogre::Entity *smallCarBodyMesh;
    Ogre::Entity *smallCarLDoorMesh;
    Ogre::Entity *smallCarRDoorMesh;
    Ogre::Entity *smallCarFBumperMesh;
    Ogre::Entity *smallCarRBumperMesh;
    Ogre::Entity *bangerBodyMesh;
    Ogre::Entity *bangerFLDoorMesh;
    Ogre::Entity *bangerFRDoorMesh;
    Ogre::Entity *bangerRLDoorMesh;
    Ogre::Entity *bangerRRDoorMesh;
    Ogre::Entity *bangerFBumperMesh;
    Ogre::Entity *bangerRBumperMesh;
    Ogre::Entity *truckBodyMesh;
    Ogre::Entity *truckLDoorMesh;
    Ogre::Entity *truckRDoorMesh;
    Ogre::Entity *truckRBumperMesh;

    MeshDeformer*  mMeshDeformer;
    Ogre::RenderWindow* mWindow;
	CEGUI::Window* mGUIWindow;

    btRigidBody *mArenaBody;

    static bool guiSetup;

protected:
    void setupCompositorChain (Ogre::Viewport* vp);
    void setupShadowSystem (void);
    void setupLightSystem (void);
    void setupParticleSystem (void);
    void setupArenaNodes (void);

    void loadArenaGraphics (ArenaID aid);
    void unloadArenaGraphics (ArenaID aid);
    void loadArenaPhysics (ArenaID aid);
    void unloadArenaPhysics (ArenaID aid);
    void loadArenaLighting (ArenaID aid);
    
    void setupMeshDeformer (void);
    virtual void setupGUI (void);
    virtual void setupUserInput (void) = 0;

#ifdef COMPOSITOR_MOTION_BLUR
    void createMotionBlurCompositor (void);
#endif
    
    // GUI elements which are setup.
    CEGUI::OgreRenderer* mGUIRenderer;

    // Scene elements which are setup.
    Ogre::SceneNode* arenaNode;
    Ogre::Light*     mWorldSun;
    Ogre::SceneNode* mVIPIcon[2]; ///< This is completely not the place for this but I'm waiting
                                  //   for a better place (in the team class for example) to be 
                                  //   made available.
    // Particle Systems and their configuration parameters
#ifdef PARTICLE_EFFECT_SPARKS
    Ogre::ParticleSystem* mSparkSystem;
    Ogre::NameValuePairList mSparkParams;
#endif
#ifdef PARTICLE_EFFECT_SPLINTERS
    Ogre::ParticleSystem* mSplinterSystem;
    Ogre::NameValuePairList mSplinterParams;
#endif
#ifdef PARTICLE_EFFECT_EXPLOSION
    Ogre::ParticleSystem* mExplosionNucleusSystem;
    Ogre::ParticleSystem* mExplosionSmokeSystem;
    Ogre::ParticleSystem* mExplosionDebrisSystem;
    Ogre::NameValuePairList mExplosionNucleusParams;
    Ogre::NameValuePairList mExplosionSmokeParams;
    Ogre::NameValuePairList mExplosionDebrisParams;
#endif
#ifdef PARTICLE_EFFECT_SHRAPNEL
    #ifdef PARTICLE_EFFECT_SHRAPNEL_HIQUAL
        std::queue<Ogre::ParticleSystem*> mShrapnelSystems;
    #else
        Ogre::ParticleSystem*   mShrapnelSystem;
        Ogre::NameValuePairList mShrapnelParams;
    #endif

    #ifdef PARTICLE_EFFECT_SHRAPNEL_SPARKS
        Ogre::ParticleSystem*   mShrapnelSparkSystem;
        Ogre::NameValuePairList mShrapnelSparkParams;
    #endif
#endif
    
    // Compositor logic modules
#ifdef COMPOSITOR_HDR
    HDRLogic*        mHDRLogic;
#endif
#ifdef COMPOSITOR_BLOOM
    BloomLogic*      mBloomLogic;
#endif
#ifdef COMPOSITOR_MOTION_BLUR
    MotionBlurLogic* mMotionBlurLogic;
#endif
#ifdef COMPOSITOR_RADIAL_BLUR
    RadialBlurLogic* mRadialBlurLogic;
#endif
};

#endif
