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
  Method:       GraphicsApplication::createScene(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Loads and adds all entities to the scene.
 ------------------------------------------------------------------------------*/
void GraphicsApplication::createScene(void)
{
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));

    Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
    Ogre::SceneNode* ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
    ninjaNode->attachObject(ninjaEntity);

    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(250, 150, 250));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
}


bool GraphicsApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = GraphicsCore::frameRenderingQueued(evt);
  
    if (!processUnbufferedInput(evt))
        return false;
    return ret;
}


bool GraphicsApplication::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    static bool mMouseDown = false;     // If a mouse button is depressed
    static Ogre::Real mToggle = 0.0;    // The time left until next toggle
    static Ogre::Real mRotate = 0.13;   // The rotate constant
    static Ogre::Real mMove = 250;      // The movement constant
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);

    if (currMouse && !mMouseDown)
    {
        Ogre::Light* light = mSceneMgr->getLight("pointLight");
        light->setVisible(!light->isVisible());
    }
    mMouseDown = currMouse;

    Ogre::Vector3 transVector = Ogre::Vector3::ZERO;
    if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
    {
        transVector.z -= mMove;
    }
    if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
    {
        transVector.z += mMove;
    }
    if (mKeyboard->isKeyDown(OIS::KC_J)) // Left - yaw or strafe
    {
        if(mKeyboard->isKeyDown( OIS::KC_LSHIFT ))
        {
            // Yaw left
            mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(mRotate * 5));
        } else {
            transVector.x -= mMove; // Strafe left
        }
    }
    if (mKeyboard->isKeyDown(OIS::KC_L)) // Right - yaw or strafe
    {
        if(mKeyboard->isKeyDown( OIS::KC_LSHIFT ))
        {
            // Yaw right
            mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(-mRotate * 5));
        } else {
            transVector.x += mMove; // Strafe right
        }
    }
    if (mKeyboard->isKeyDown(OIS::KC_U)) // Up
    {
        transVector.y += mMove;
    }
    if (mKeyboard->isKeyDown(OIS::KC_O)) // Down
    {
        transVector.y -= mMove;
    }

    mSceneMgr->getSceneNode("NinjaNode")->translate(transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

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
