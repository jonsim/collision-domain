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



// Definitions
// Particle effect definitions.
// global
#define PARTICLE_EFFECT_VIPSTREAK   // From VIP crowns.
#define PARTICLE_EFFECT_EXPLOSION   // On death.
#define PARTICLE_EFFECT_SHRAPNEL    // From crunching collisions.
//#define PARTICLE_EFFECT_SHRAPNEL_SPARKS
//#define PARTICLE_EFFECT_SPARKS      // From scraping collisions.
// local
#define PARTICLE_EFFECT_DUST        // From skidding etc.
#define PARTICLE_EFFECT_EXHAUST     // From cars bums.
#define PARTICLE_EFFECT_SMOKE       // From damaged cars.
//#define PARTICLE_EFFECT_FIRE        // From dead cars.

// Enabled high quality (colliding) shrapnel effect. PARTICLE_EFFECT_SHRAPNEL must be defined for this to have any effect.
// A low quality, non-colliding version is used if this is not defined but shrapnel is enabled.
//#define PARTICLE_EFFECT_SHRAPNEL_HIQUAL



// Classes
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

    void loadHDR (Ogre::Viewport* vp, uint8_t mode);
    void loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight);
    void loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur);
    void setRadialBlur (Ogre::Viewport* vp, float blur);

    void createArenaCollisionShapes();
    void loadArena (ArenaID aid);
    void unloadArena (ArenaID aid);
    // Graphical effect settings. Adjusts the scale of the effect - default is 1.
    float mGfxSettingHDR;         // Strength of the High Dynamic Range lighting.
    float mGfxSettingBloom;       // Strength of the bloom lighting artifact.
    float mGfxSettingMotionBlur;  // Amount of blurring of relatively moving objects.
    float mGfxSettingRadialBlur;  // Amount of blurring encountered at high speeds.

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

    void createMotionBlurCompositor (void);
    
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
    HDRLogic*        mHDRLogic;
    BloomLogic*      mBloomLogic;
    MotionBlurLogic* mMotionBlurLogic;
    RadialBlurLogic* mRadialBlurLogic;
};

#endif
