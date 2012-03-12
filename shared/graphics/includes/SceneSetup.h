/**
 * @file	SceneSetup.h
 * @brief 	Sets up the scene
 */
#ifndef SCENESETUP_H
#define SCENESETUP_H

#include "stdafx.h"
#include "SharedIncludes.h"


/**
 *  @brief 	Will contain PlayerPool, PhysicsCore, AudioCore etc.
 */
class SceneSetup
{
public:
    SceneSetup (void);
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

protected:
	void setupCompositorChain (Ogre::Viewport* vp);
    void setupShadowSystem (void);
    void setupLightSystem (void);
    void setupParticleSystem (void);
    void setupArena (void);
    virtual void setupGUI (void);

	void createMotionBlurCompositor (void);
    
    // GUI elements which are setup.
	CEGUI::OgreRenderer* mGUIRenderer;

    // Scene elements which are setup.
	Ogre::Light*          mWorldSun;
    Ogre::SceneNode*      mVIPIcon[2]; ///< This is completely not the place for this but I'm waiting
                                      //   for a better place (in the team class for example) to be 
                                      //   made available.
    // Particle Systems
	Ogre::ParticleSystem* mWeatherSystem;
	Ogre::ParticleSystem* mSparkSystem;
    Ogre::ParticleSystem* mExplosionNucleusSystem;
    Ogre::ParticleSystem* mExplosionSmokeSystem;
    Ogre::ParticleSystem* mExplosionDebrisSystem;
    // Particle System configurations
    Ogre::NameValuePairList mSparkParams;
    Ogre::NameValuePairList mExplosionNucleusParams;
    Ogre::NameValuePairList mExplosionSmokeParams;
    Ogre::NameValuePairList mExplosionDebrisParams;
    
	// Compositor logic modules
	HDRLogic*		 mHDRLogic;
	BloomLogic*		 mBloomLogic;
	MotionBlurLogic* mMotionBlurLogic;
	RadialBlurLogic* mRadialBlurLogic;
};

#endif
