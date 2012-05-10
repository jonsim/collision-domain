/**
 * @file    SceneSetup.h
 * @brief     Sets up the scene
 */
#ifndef SCENESETUP_H
#define SCENESETUP_H

#include "stdafx.h"
#include "PostFilterLogic.h"
#include <stdint.h>

class MeshDeformer;
/**
 *  @brief     Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */

class SceneSetup
{
public:
    SceneSetup (int arenaChoice);
    virtual ~SceneSetup (void);

    void loadHDR (Ogre::Viewport* vp, uint8_t mode);
    void loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight);
    void loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur);
    void setRadialBlur (Ogre::Viewport* vp, float blur);
    void setWeather (uint8_t mode);

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

    int getArenaNumber() { return this->mArenaChoice; }
    void incrementArenaNumber() { this->mArenaChoice++; }
    //int setArenaNumber(int newChoice) { this->mArenaChoice = newChoice);


protected:
    void setupCompositorChain (Ogre::Viewport* vp);
    void setupShadowSystem (void);
    void setupLightSystem (void);
    void setupParticleSystem (void);
    void setupArenaNodes (void);
    void setupArenaPhysics (void);
    void setupArenaGraphics (void);
    void setupMeshDeformer (void);
    virtual void setupGUI (void);
    virtual void setupUserInput (void) = 0;

    void createMotionBlurCompositor (void);
    
    // GUI elements which are setup.
    CEGUI::OgreRenderer* mGUIRenderer;

    // Scene elements which are setup.
    Ogre::Light*     mWorldSun;
    Ogre::SceneNode* mVIPIcon[2]; ///< This is completely not the place for this but I'm waiting
                                  //   for a better place (in the team class for example) to be 
                                  //   made available.
    // Particle Systems
    Ogre::ParticleSystem* mWeatherSystem;
    Ogre::ParticleSystem* mSparkSystem;
    Ogre::ParticleSystem* mExplosionNucleusSystem;
    Ogre::ParticleSystem* mExplosionSmokeSystem;
    Ogre::ParticleSystem* mExplosionDebrisSystem;
    std::queue<Ogre::ParticleSystem*> mShrapnelSystems;
    // Particle System configurations
    Ogre::NameValuePairList mSparkParams;
    Ogre::NameValuePairList mExplosionNucleusParams;
    Ogre::NameValuePairList mExplosionSmokeParams;
    Ogre::NameValuePairList mExplosionDebrisParams;
    
    // Compositor logic modules
    HDRLogic*        mHDRLogic;
    BloomLogic*      mBloomLogic;
    MotionBlurLogic* mMotionBlurLogic;
    RadialBlurLogic* mRadialBlurLogic;

    //Current scene being used
    int mArenaChoice;
};

#endif
