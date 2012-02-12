/**
 * @file	GraphicsApplication.h
 * @brief 	Adds objects to the graphics interface and contains the framelistener
 */
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


/*-------------------- DEFINITIONS --------------------*/
#define GFX_EFFECT_HDR
#define GFX_EFFECT_BLOOM
#define GFX_EFFECT_RADIAL_BLUR
#define GFX_EFFECT_MOTION_BLUR


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief  Adds objects to the graphics interface.
 *
 *          Derived from the Ogre Tutorial Framework (TutorialApplication.h).
 */
class GraphicsApplication : public GraphicsCore
{
public:
    GraphicsApplication (void);
    virtual ~GraphicsApplication (void);
	
	// Graphical effect managers
	void loadHDR (uint8_t mode);
	void loadBloom (uint8_t mode, float blurWeight, float originalWeight);
	void loadMotionBlur (uint8_t mode, float blur);
	void setRadialBlur (float blur);
	void setWeather (uint8_t mode);
	void startBenchmark (uint8_t stage);
	
	// Graphical effect settings. Adjusts the scale of the effect - default is 1.
	float gfxSettingHDR;         // Strength of the High Dynamic Range lighting.
	float gfxSettingBloom;       // Strength of the bloom lighting artifact.
	float gfxSettingMotionBlur;  // Amount of blurring of relatively moving objects.
	float gfxSettingRadialBlur;  // Amount of blurring encountered at high speeds.

protected:
    virtual void createScene (void);
    virtual void createFrameListener (void);
	
    // Ogre::FrameListener overrides.
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);
    
    // GUI Manager.
	CEGUI::OgreRenderer* mGuiRenderer;

private:
	// Setup functions.
	void setupCompositorChain (void);
	void setupShadowSystem (void);
    void setupLighting (void);
	void setupParticles (void);
    void setupArena (void);
	void setupGUI (void);

	// GUI functions.
	void createSpeedo (void);
    void createGearDisplay (void);
	void updateSpeedo (float fSpeed, int iGear);

	void finishBenchmark (uint8_t stage, float averageTriangles);
	void createMotionBlurCompositor (void);

	Ogre::OverlayContainer *olcSpeedo;
	Ogre::OverlayElement   *oleNeedle;
    Ogre::OverlayElement   *oleGear;

	Ogre::Light*          mWorldSun;
	Ogre::ParticleSystem* mWeatherSystem;
	Ogre::ParticleSystem* mSparkSystem;

	bool    mBenchmarkRunning;
	uint8_t mBenchmarkStage;

	// Compositor logic modules
	HDRLogic*		 mHDRLogic;
	BloomLogic*		 mBloomLogic;
	MotionBlurLogic* mMotionBlurLogic;
	RadialBlurLogic* mRadialBlurLogic;
};

#endif // #ifndef GRAPHICSAPPLICATION_H
