/**
 * @file	GraphicsApplication.h
 * @brief 	Adds objects to the graphics interface and contains the framelistener
 */
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


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
    
	// Extra server gameplay shit. This shouldn't be here. Will be purged.
    Ogre::Camera* mViewCam1;
    Ogre::Camera* mViewCam2;
    BigScreen* bigScreen;
    ViewportManager* vpm;

protected:
    virtual void createCamera (void);
    virtual void createViewports (void);

    virtual void createScene (void);
    virtual void createFrameListener (void);
	
    // Ogre::FrameListener overrides
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);

    // GUI Manager.
	CEGUI::OgreRenderer* mGuiRenderer;

private:
    // Setup functions.
    void setupLighting (uint8_t mode);
    void setupArena (void);

    // GUI functions.
};

#endif // #ifndef GRAPHICSAPPLICATION_H
