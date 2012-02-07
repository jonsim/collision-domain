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
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

    int clientID; ///< The client ID which is assigned by the server.
    bool firstFrameOccurred;
	
	void setWeatherMode (uint8_t mode);
	void hdrLoader (uint8_t mode);
	void bloomLoader (uint8_t mode, float blurWeight, float originalWeight);
	void motionBlurLoader (uint8_t mode, float blur);
	void setRadialBlur (float blur);
	void startBenchmark (uint8_t stage);
	
	// Graphics settings. Set these to 0 to disable the effect. Defaults are all 1.
	float gfxSettingHDR;			// Strength of the High Dynamic Range lighting.
	float gfxSettingBloom;			// Strength of the bloom lighting artifact.
	float gfxSettingMotionBlur;		// Amount of blurring of relatively moving objects.
	float gfxSettingRadialBlur;		// Amount of blurring encountered at high speeds.

protected:
	CEGUI::OgreRenderer* mGuiRenderer;

    virtual void createScene(void);
    virtual void createFrameListener(void);
	
    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool frameStarted(const Ogre::FrameEvent& evt);
    virtual bool frameEnded(const Ogre::FrameEvent& evt);
	

private:
	void setupCompositorChain (void);
	void setupShadowSystem (void);
    void setupLighting (void);
	void setupParticles (void);
    void setupArena (void);
    void setupNetworking (void);
	void createSpeedo (void);
    void createGearDisplay (void);
	void updateSpeedo (float fSpeed, int iGear);
	void createMotionBlurCompositor (void);
	void finishBenchmark (uint8_t stage, float averageTriangles);

	Ogre::OverlayContainer *olcSpeedo;
	Ogre::OverlayElement *oleNeedle;
    Ogre::OverlayElement *oleGear;

	Ogre::Light* worldSun;
	Ogre::ParticleSystem* weatherSystem;
	bool benchmarkRunning;
	uint8_t benchmarkStage;

	// Compositor logic modules
	HDRLogic*		 hdrLogic;
	BloomLogic*		 bloomLogic;
	MotionBlurLogic* motionBlurLogic;
	RadialBlurLogic* radialBlurLogic;
};

#endif // #ifndef GRAPHICSAPPLICATION_H
