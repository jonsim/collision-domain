/*------------------------------------------------------------------------------
  File:     GraphicsApplication.cpp
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "stdafx.h"
#include "GraphicsApplication.h"



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::GraphicsApplication(void)
{
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::~GraphicsApplication(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
GraphicsApplication::~GraphicsApplication(void)
{
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createCamera(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Creates and positions the scene's camera.
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createCamera(void)
{
    // create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // set its position, direction  
    mCamera->setPosition(Ogre::Vector3(0,50,200));
    mCamera->lookAt(Ogre::Vector3(0,0,0));

    // set the near clip distance
    mCamera->setNearClipDistance(5);

    // create a default camera controller
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createViewports(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Creates and scales the scene's viewport(s).
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);

    // set the background to black
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}


/*------------------------------------------------------------------------------
  Method:       GraphicsApplication::createScene(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Loads and adds all entities to the scene.
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createScene(void)
{
    // create entities
	Ogre::Entity* ogreHead1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
    Ogre::Entity* ogreHead2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
    ogreHead1->setCastShadows(true);
    ogreHead2->setCastShadows(true);

    // create the ground entity
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane("GroundMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
    Ogre::Entity* ground = mSceneMgr->createEntity("Ground", "GroundMesh");
    ground->setMaterialName("Examples/Rockwall");
    ground->setCastShadows(false);

    // create scene nodes for the entities
	Ogre::SceneNode* headNode1  = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode1", Ogre::Vector3(-50, 25, 0));
    Ogre::SceneNode* headNode2  = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode2", Ogre::Vector3(50, 25, 0));
    Ogre::SceneNode* groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode", Ogre::Vector3(0, 0, 0));

    // attach the entities
	headNode1->attachObject(ogreHead1);
    headNode2->attachObject(ogreHead2);
    groundNode->attachObject(ground);
    
    // Set the shadow renderer
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
     
	// Create the lights
	Ogre::Light* pointLight1 = mSceneMgr->createLight("pointLight1");
    pointLight1->setType(Ogre::Light::LT_POINT);
	pointLight1->setPosition(0, 100, 0);

    Ogre::Light* directionalLight1 = mSceneMgr->createLight("directionalLight1");
    directionalLight1->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight1->setDiffuseColour( Ogre::ColourValue(0.25, 0.25, 0));
    directionalLight1->setSpecularColour(Ogre::ColourValue(0.25, 0.25, 0));
    directionalLight1->setDirection(Ogre::Vector3(0, -1, 1));
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
