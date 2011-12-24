/**
 * @file    GraphicsApplication.cpp
 * @brief     Adds objects to the graphics interface.
 *          Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GraphicsApplication.h"
/*#include <stdio.h> // for showWin32Console()
#include <fcntl.h>
#include <io.h>
#include <iostream>*/


/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor.
GraphicsApplication::GraphicsApplication (void)
{
}


/// @brief  Destructor.
GraphicsApplication::~GraphicsApplication (void)
{
}


/// @brief  Creates the initial scene prior to the first render pass, adding objects etc.
void GraphicsApplication::createScene (void)
{
    setupLighting();
    setupArena();
    setupNetworking();

    // Load the ninjas
    Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
    ninjaEntity->setCastShadows(true);
    Ogre::SceneNode* ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    ninjaNode->attachObject(ninjaEntity);
    ninjaNode->translate(0, 0, 0);
    Ogre::Entity* ninjaEntity2 = mSceneMgr->createEntity("Ninja2", "ninja.mesh");
    ninjaEntity2->setCastShadows(true);
    Ogre::SceneNode* ninjaNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode2");
    ninjaNode2->attachObject(ninjaEntity2);
    ninjaNode2->pitch(Ogre::Degree(90));
    ninjaNode2->roll(Ogre::Degree(180));
    ninjaNode2->translate(0, 100, 0);
    
    // Add all players
    players[clientID].createPlayer(mSceneMgr, MEDIUM, SKIN0, mPhysicsCore);

    // Attach a camera to the first player
    players[clientID].attachCamera(mCamera);
}


/// @brief  Adds and configures lights to the scene.
void GraphicsApplication::setupLighting (void)
{
    // Set the ambient light.
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25f, 0.25f, 0.25f));
    
    // Add a directional light
    Ogre::Vector3 directionalLightDir(0.55f, -0.3f, 0.75f);
    directionalLightDir.normalise();
    Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour( Ogre::ColourValue::White);
    directionalLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
    directionalLight->setDirection(directionalLightDir);
    
    // Create the skybox
    mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

    // Set the shadow renderer
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}


/// @brief  Builds the initial arena.
void GraphicsApplication::setupArena (void)
{
    // Create the ground plane and wall meshes
    Ogre::Plane groundPlane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane("GroundMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 5000, 20, 20, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);
    Ogre::Plane wallPlane1(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton().createPlane("WallMesh1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 200, 20, 1, true, 1, 20, 1, Ogre::Vector3::UNIT_Z);
    Ogre::Plane wallPlane2(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton().createPlane("WallMesh2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 200, 20, 1, true, 1, 20, 1, Ogre::Vector3::UNIT_Z);
    Ogre::Plane wallPlane3(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton().createPlane("WallMesh3", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 200, 20, 1, true, 1, 20, 1, Ogre::Vector3::UNIT_Z);
    Ogre::Plane wallPlane4(Ogre::Vector3::UNIT_Z, 0);
    Ogre::MeshManager::getSingleton().createPlane("WallMesh4", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, groundPlane, 5000, 200, 20, 1, true, 1, 20, 1, Ogre::Vector3::UNIT_Z);

    // Load and meshes and create entities
    Ogre::Entity* groundEntity = mSceneMgr->createEntity("Ground", "GroundMesh");
    groundEntity->setMaterialName("Examples/GrassFloor");
    groundEntity->setCastShadows(false);
    Ogre::Entity* wallEntity1 = mSceneMgr->createEntity("Wall1", "WallMesh1");
    wallEntity1->setMaterialName("Examples/Rockwall");
    wallEntity1->setCastShadows(true);
    Ogre::Entity* wallEntity2 = mSceneMgr->createEntity("Wall2", "WallMesh1");
    wallEntity2->setMaterialName("Examples/Rockwall");
    wallEntity2->setCastShadows(true);
    Ogre::Entity* wallEntity3 = mSceneMgr->createEntity("Wall3", "WallMesh1");
    wallEntity3->setMaterialName("Examples/Rockwall");
    wallEntity3->setCastShadows(true);
    Ogre::Entity* wallEntity4 = mSceneMgr->createEntity("Wall4", "WallMesh1");
    wallEntity4->setMaterialName("Examples/Rockwall");
    wallEntity4->setCastShadows(true);

    // Create scene nodes and attach the entities
    Ogre::SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode", Ogre::Vector3(0, 0, 0));
    groundNode->attachObject(groundEntity);
    Ogre::SceneNode* wallNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("WallNode1", Ogre::Vector3(0, 100, 2500));
    wallNode1->attachObject(wallEntity1);
    Ogre::SceneNode* wallNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("WallNode2", Ogre::Vector3(2500, 100, 0));
    wallNode2->attachObject(wallEntity2);
    Ogre::SceneNode* wallNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode("WallNode3", Ogre::Vector3(0, 100, -2500));
    wallNode3->attachObject(wallEntity3);
    Ogre::SceneNode* wallNode4 = mSceneMgr->getRootSceneNode()->createChildSceneNode("WallNode4", Ogre::Vector3(-2500, 100, 0));
    wallNode4->attachObject(wallEntity4);

    // Adjust the node rotations.
    wallNode1->pitch(Ogre::Degree(-90));
    wallNode3->pitch(Ogre::Degree(90));
    wallNode2->pitch(Ogre::Degree(90));
    wallNode2->roll(Ogre::Degree(90));
    wallNode4->pitch(Ogre::Degree(-90));
    wallNode4->roll(Ogre::Degree(-90));

    // create collideable floor so shit doesn't freefall. It will hit the floor.
    mPhysicsCore->createFloorPlane();
    mPhysicsCore->createWallPlanes();
}


/// @brief  Configures the networking, retreiving the required data from the server.
void GraphicsApplication::setupNetworking (void)
{
    //clientID = server.allocateClientID();
    clientID = 0;
}


/// @brief  Passes the frame listener down to the GraphicsCore.
void GraphicsApplication::createFrameListener (void)
{
    GraphicsCore::createFrameListener();
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
bool GraphicsApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    // MUST BE THE FIRST THING - do the core things (GraphicsCore is extended by this class)
    if (!GraphicsCore::frameRenderingQueued(evt)) return false;
    
    // Toggle on screen widgets
    if (mUserInput.isToggleWidget()) 
    {
        mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
        mDetailsPanel->show();
    }
    
    // NOW WE WILL DO EVERYTHING BASED OFF THE LATEST KEYBOARD / MOUSE INPUT

    // Process keyboard input and produce an InputState object from this.
    InputState *inputSnapshot = mUserInput.getInputState();
    
    // Fire network data (must be after player has had controls applied).
    mNetworkCore->frameEvent(inputSnapshot);

    // Apply controls the player (who will be moved on frameEnd and frameStart).
    players[clientID].processControlsFrameEvent(inputSnapshot, evt.timeSinceLastFrame, 1./60.);
    players[clientID].updateCameraFrameEvent(mUserInput.getMouseXRel(), mUserInput.getMouseYRel());

    // Perform Client Side Prediction.
    // Move any players who are out of sync
    
    /* Deal with all but local player (who's snapshots should be 0ms behind where this client thinks they are)
    for (i : otherPlayerIDs)
    {
        CarSnapshot *carSnapshot = getCarSnapshotIfExistsSincePreviousGet(int playerID);

        if (CSP.needsPushingBackIntoPosition(players[playerID], carSnapshot)
        {
            players[playerID].getCar()->restoreSnapshot(carSnapshot);
        }

        delete carSnapshot;
    }
    
    // Deal with the local player (who's snapshot will be x=latency ms behind the real thing)
    if (CSP.needsMePushedBack(players[clientID], carSnapshot))
    {
        // Calculate a snapshot which doesn't jolt the player harshly if it can be fixed with small movements
        CarSnapshot *latestPlayerSnapshot = getCarSnapshotIfExistsSincePreviousGet(int clientID);
        CarSnapshot *fixSnapshot = new CarSnapshot(...);

        players[clientID].getCar->restoreSnapshot(fixSnapshot);
    }
    */

    // Cleanup frame specific objects so we don't rape memory. If you want to remember some, delete them later!
    delete inputSnapshot;

    return true;
}


bool GraphicsApplication::frameStarted(const Ogre::FrameEvent& evt)
{

    // stepSimulation proceeds the simulation over 'timeStep', units in preferably in seconds.
    // By default, Bullet will subdivide the timestep in constant substeps of each 'fixedTimeStep'.
    // in order to keep the simulation real-time, the maximum number of substeps can be clamped to
    // 'maxSubSteps'. You can disable subdividing the timestep/substepping by passing maxSubSteps=0
    // as second argument to stepSimulation, but in that case you have to keep the timeStep constant.
    // J - If framerate is low (<15fps) it may be valid to make fixedTimeStep a larger number (at the
    // expense of physics accuracy. Server will be correcting this though).
    // maxSubSteps = 7.5 - this will do up to 1/8 second's worth of processing here and 1/8 second's worth
    // processing in frameEnded. so minumum frame rate of 4fps before physics will become innacurate
    // and rely on the server to solve.

    mPhysicsCore->mWorld->stepSimulation(/*timeStep*/evt.timeSinceLastFrame, /*maxSubSteps*/7, /*fixedTimeStep*/1./60.);
    return true;
}


bool GraphicsApplication::frameEnded(const Ogre::FrameEvent& evt)
{
    mPhysicsCore->mWorld->stepSimulation(evt.timeSinceLastFrame, /*maxSubSteps*/7, /*fixedTimeStep*/1./60.);   // update Bullet Physics animation
    return true;
}


// The following code is not understood. Does something for Win32: unknown. Best just leave it alone.
#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        GraphicsApplication app;

        //showWin32Console();

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif



/*
void showWin32Console()
{
    static const WORD MAX_CONSOLE_LINES = 500;
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    // allocate a console for this app
    AllocConsole();
    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
    coninfo.dwSize);
    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );
    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();
}
*/