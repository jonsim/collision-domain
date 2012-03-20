/**
 * @file	GraphicsCore.h
 * @brief 	Configures the graphical settings and provides the common graphical functionality.
 */
#ifndef GRAPHICSCORE_H
#define GRAPHICSCORE_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief 	Builds the window and provides the necessary low level functions.
 */
class SpawnScreen;

class MeshDeformer;

class GraphicsCore : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public SceneSetup
{
public:
    GraphicsCore (void);
    virtual ~GraphicsCore (void);
    virtual void go (void);
	void shutdown() { mShutDown = true; }
    void generateExplosion (Ogre::Vector3 location);
    void generateSparks (Ogre::Vector3 location, Ogre::Vector3 direction);
    void updateVIPLocation (int teamNumber, Ogre::Vector3 location);

    Ogre::Camera*       mCamera;
    Ogre::RenderWindow* mWindow;
    SpawnScreen *mSpawnScreen;	// This shouldn't be here (Input.cpp is shared). Will be purged.

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
	MeshDeformer *meshDeformer;
    

protected:
    virtual bool initApplication (void); // This shouldn't be here. Will be purged.
    virtual bool configureRenderer (void);
    virtual void createCamera (void) = 0;
    virtual void createFrameListener (void);
    virtual void createScene (void) = 0;     // This needs to be overridden to display anything
    virtual void destroyScene (void);
    virtual void createViewports (void) = 0;
    virtual void setupResources (void);
    virtual void createResourceListener (void);
    virtual void loadResources (void);
    virtual void updateParticleSystems (void);

    // Ogre::FrameListener overrides.
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    
    // Ogre::WindowEventListener overrides.
    virtual void windowResized (Ogre::RenderWindow* rw);
    virtual void windowClosed (Ogre::RenderWindow* rw);

    // OIS user input
    Input mUserInput;

    Ogre::Root*  mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    // OgreBites
    OgreBites::SdkCameraMan* mCameraMan;     // basic camera controller
    bool mCursorWasVisible;                  // Was the cursor visible before dialog appeared
    bool mDebrisVisible;
    bool mShutDown;
};

#endif // #ifndef GRAPHICSCORE_H
