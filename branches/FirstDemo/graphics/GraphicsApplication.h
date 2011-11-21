/*------------------------------------------------------------------------------
  File:     GraphicsApplication.h
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.h).
 ------------------------------------------------------------------------------*/
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "stdafx.h"
#include "GraphicsCore.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h> 



/******************** CLASS DEFINITIONS ********************/
class GraphicsApplication : public GraphicsCore
{
private:
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    OgreBites::Label* mInfoLabel;
    bool mTerrainsImported;

    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);

public:
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

protected:
    virtual void createScene(void);
	//virtual void createCamera(void);
	//virtual void createViewports(void);
    virtual void createFrameListener(void);
    virtual void destroyScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // #ifndef GRAPHICSAPPLICATION_H
