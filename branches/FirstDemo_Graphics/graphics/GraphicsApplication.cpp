/*------------------------------------------------------------------------------
  File:     GraphicsApplication.cpp
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "includes/stdafx.h"
#include "includes/GraphicsApplication.h"
#include "../networking/includes/fakeServer.h"
#include "../base/includes/playerSnapshot.h"



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::GraphicsApplication (void)
{
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::~GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::~GraphicsApplication (void)
{
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createScene(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Loads and adds all entities to the scene.
 ------------------------------------------------------------------------------*/
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
    clientPlayerList[0].createPlayer(mSceneMgr, MEDIUM, SKIN0);

    // Attach a camera to the first player
    clientPlayerList[0].attachCamera(mCamera);
}


void GraphicsApplication::setupLighting (void)
{
    // Set the ambient light.
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
    
    // Add a directional light
    Ogre::Vector3 directionalLightDir(0.55, -0.3, 0.75);
    directionalLightDir.normalise();
    Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour( Ogre::ColourValue::White);
    directionalLight->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    directionalLight->setDirection(directionalLightDir);
    
    // Create the skybox
    mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

    // Set the shadow renderer
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}


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
}


void GraphicsApplication::setupNetworking (void)
{
    clientID = server.allocateClientID();
}


void GraphicsApplication::createFrameListener (void)
{
	GraphicsCore::createFrameListener();
}


bool GraphicsApplication::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
    if (mWindow->isClosed())
        return false;
    mKeyboard->capture();
    mMouse->capture();
    mTrayMgr->frameRenderingQueued(evt);

    playerKeyState forwardState = (playerKeyState) 0;
    playerKeyState turnState    = (playerKeyState) 0;

    // Process keyboard input and derive playerKeyStates from this.
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;
    if (mKeyboard->isKeyDown(OIS::KC_W))
        forwardState = (playerKeyState) ((int) forwardState + 1);
    if (mKeyboard->isKeyDown(OIS::KC_S))
        forwardState = (playerKeyState) ((int) forwardState - 1);
    if (mKeyboard->isKeyDown(OIS::KC_A))
        turnState    = (playerKeyState) ((int) turnState + 1);
    if (mKeyboard->isKeyDown(OIS::KC_D))
        turnState    = (playerKeyState) ((int) turnState - 1);

    clientPlayerList[0].calculateState(playerSnapshot(forwardState, turnState), evt.timeSinceLastFrame);

    return true;
}


/*bool GraphicsApplication::keyPressed (const OIS::KeyEvent& evt)
{
    switch (evt.key)
    {
        case OIS::KC_ESCAPE: 
            mShutDown = true;
            break;
        /*case OIS::KC_UP:
        case OIS::KC_W:
            carAcceleration = -carAccelerationConstant;
            break;
        case OIS::KC_DOWN:
        case OIS::KC_S:
            carAcceleration = carAccelerationConstant;
            break;
        case OIS::KC_LEFT:
        case OIS::KC_A:
            carRotation = carTurningConstant;
            break;
        case OIS::KC_RIGHT:
        case OIS::KC_D:
            carRotation = -carTurningConstant;
            break;*
        default:
            break;
    }

    return true;
}


bool GraphicsApplication::keyReleased (const OIS::KeyEvent& evt)
{
    switch (evt.key)
    {
        /*case OIS::KC_UP:
        case OIS::KC_W:
            carAcceleration = 0;
            break;
        case OIS::KC_DOWN:
        case OIS::KC_S:
            carAcceleration = 0;
            break;
        case OIS::KC_LEFT:
        case OIS::KC_A:
            carRotation = 0;
            break;
        case OIS::KC_RIGHT:
        case OIS::KC_D:
            carRotation = 0;
            break;*
        default:
            break;
    }

    return true;
}*/


bool GraphicsApplication::mouseMoved (const OIS::MouseEvent& evt)
{
    return true;
}


bool GraphicsApplication::mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    return true;
}


bool GraphicsApplication::mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    return true;
}


/*------------------------------------------------------------------------------
  Method:       INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
  Parameters:   Unknown.
  Outputs:      Unknown.
  Purpose:      Does something for Win32. Unknown.
 ------------------------------------------------------------------------------*/
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
