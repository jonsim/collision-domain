/*------------------------------------------------------------------------------
  File:     GraphicsApplication.cpp
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.cpp).
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/

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
  Method:       GraphicsApplication::createScene(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Loads and adds all entities to the scene.
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createScene(void)
{
	Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
 
	Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(ogreHead);
 
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
 
	// Create a light
	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);
}


/*------------------------------------------------------------------------------
  Method:       INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
  Parameters:   Unknown.
  Outputs:      Unknown.
  Purpose:      Does something for Win32. Unknown.
 ------------------------------------------------------------------------------*/
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

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
