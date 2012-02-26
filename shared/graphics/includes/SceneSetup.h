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

    void setWeather (uint8_t mode);
	
    void loadHDR (Ogre::Viewport* vp, uint8_t mode);
    void loadBloom (Ogre::Viewport* vp, uint8_t mode, float blurWeight, float originalWeight);
    void loadMotionBlur (Ogre::Viewport* vp, uint8_t mode, float blur);
    void setRadialBlur (Ogre::Viewport* vp, float blur);

	// Graphical effect settings. Adjusts the scale of the effect - default is 1.
	float gfxSettingHDR;         // Strength of the High Dynamic Range lighting.
	float gfxSettingBloom;       // Strength of the bloom lighting artifact.
	float gfxSettingMotionBlur;  // Amount of blurring of relatively moving objects.
	float gfxSettingRadialBlur;  // Amount of blurring encountered at high speeds.

protected:
	void setupCompositorChain (Ogre::Viewport* vp);
    void setupShadowSystem (void);
    void setupLightSystem (void);
    void setupParticleSystem (void);
    void setupArena (void);
    virtual void setupGUI (void);

	void createMotionBlurCompositor (void);
    
    // GUI elements which are setup.
	CEGUI::OgreRenderer* mGuiRenderer;

    // Scene elements which are setup.
	Ogre::Light*          mWorldSun;
	Ogre::ParticleSystem* mWeatherSystem;
	Ogre::ParticleSystem* mSparkSystem;
    
	// Compositor logic modules
	HDRLogic*		 mHDRLogic;
	BloomLogic*		 mBloomLogic;
	MotionBlurLogic* mMotionBlurLogic;
	RadialBlurLogic* mRadialBlurLogic;
};

#endif
