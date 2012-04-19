/**
 * @file	Input.cpp
 * @brief 	Controls the users input.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, initialising OIS input devices.
Input::Input () : mInputManager(0), mMouse(0), mKeyboard(0)
{
}


/// @brief  Deconstructor.
Input::~Input(void)
{
}


/// @brief  Sets up the input system, attaching the listener's event call backs to this object.
/// @param  pl  A parameter list containing  OS specific info (such as HWND and HINSTANCE for window apps), and access mode.
void Input::createInputSystem (OIS::ParamList pl)
{
    mInputManager = OIS::InputManager::createInputSystem(pl);
    
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}


/// @brief  Destroys the input system and detaches the input devices from the window.
void Input::destroyInputSystem ()
{
    if (mInputManager)
    {
        mInputManager->destroyInputObject(mMouse);
        mInputManager->destroyInputObject(mKeyboard);

        OIS::InputManager::destroyInputSystem(mInputManager);
        mInputManager = 0;
    }
}


/// @brief  Samples the keys being pressed by the keyboard at that moment in time, storing them in the input device objects.
void Input::capture ()
{
    mKeyboard->capture();
    mMouse->capture();
}


/// @brief  Constructs an InputState object based on the key state (at the time of the last Input::capture()).
/// @return The InputState object containing movement key information at the time of the previous sample.
InputState* Input::getInputState()
{
#ifdef COLLISION_DOMAIN_CLIENT
	if( NetworkCore::bConnected && !GameCore::mGui->consoleVisible() && !GameCore::mGui->chatboxVisible() )
	{
        return new InputState(mKeyboard->isKeyDown(OIS::KC_UP)    || mKeyboard->isKeyDown(OIS::KC_W),
                              mKeyboard->isKeyDown(OIS::KC_DOWN)  || mKeyboard->isKeyDown(OIS::KC_S),
                              mKeyboard->isKeyDown(OIS::KC_LEFT)  || mKeyboard->isKeyDown(OIS::KC_A),
                              mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D),
                              mKeyboard->isKeyDown(OIS::KC_SPACE) );
	}
	else
	{
		// Don't want to capture any keys (typing things) or at the select car screen
		return new InputState( false, false, false, false, false );
	}
#else
    return NULL;
#endif
}


/// @brief  Constructs an InputState object based on the key state (at the time of the last Input::capture()).
/// @return The InputState object containing movement key information at the time of the previous sample.
InputState* Input::getFreeCamInputState()
{
#ifdef COLLISION_DOMAIN_CLIENT
	if( NetworkCore::bConnected && !GameCore::mGui->consoleVisible() && !GameCore::mGui->chatboxVisible() )
	{
        return new InputState(mKeyboard->isKeyDown(OIS::KC_T),
                              mKeyboard->isKeyDown(OIS::KC_G),
                              mKeyboard->isKeyDown(OIS::KC_F),
                              mKeyboard->isKeyDown(OIS::KC_H),
                              mMouse->getMouseState().buttonDown( OIS::MB_Right ) );
    }
    else
    {
        return new InputState( false, false, false, false, false );
    }
#else
    return NULL;
#endif
}

/// @brief  Processes the interface controls, performing the correct actions if they are found pressed.
void Input::processInterfaceControls()
{
#ifdef COLLISION_DOMAIN_CLIENT
    // Check for console and chatbox - NB: they cannot be displayed simultaneously.
    if (NetworkCore::bConnected && !GameCore::mGui->consoleVisible() && !GameCore::mGui->chatboxVisible())
    {
	    if (mKeyboard->isKeyDown(OIS::KC_T))
            GameCore::mGui->toggleChatbox();
	    else if (mKeyboard->isKeyDown(OIS::KC_C))
            GameCore::mGui->toggleConsole();
//#ifdef COLLISION_DOMAIN_CLIENT
        if( GameCore::mPlayerPool->getLocalPlayer()->getAlive() == false )
        {
            if( mKeyboard->isKeyDown( OIS::KC_TAB ) )
            {
                GameCore::mPlayerPool->spectateNext();
            }
        }
//#endif
    }
#endif
}

/// @brief  Deals with mouse input.
/// @return The amount the mouse has moved in the X direction.
int Input::getMouseXRel()
{
    return mMouse->getMouseState().X.rel;
}


/// @brief  Deals with mouse input.
/// @return The amount the mouse has moved in the Y direction.
int Input::getMouseYRel()
{
    return mMouse->getMouseState().Y.rel;
}


/// @brief  Deals with mouse input.
/// @return The amount the mouse has moved in the Z direction (via the scroll wheel).
int Input::getMouseZRel()
{
	return mMouse->getMouseState().Z.rel;
}


/// @brief  Called whenever a keyboard key is pressed.
/// @param  evt  The KeyEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::keyPressed (const OIS::KeyEvent &evt)
{
	// Get the GUI system and inject the key press
	CEGUI::System& sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(evt.key);

	// Inject text seperately (for multi-lang keyboards)
	sys.injectChar(evt.text);
    
    // Had to put this in here for now, and the define.. because of shared includes.
#ifdef COLLISION_DOMAIN_CLIENT
    // This is safe, mSpawnScreen will only ever be not null if we are selecting
    if( GameCore::mClientGraphics->mSpawnScreen != NULL )
    {
        if( evt.key == OIS::KC_LEFT || evt.key == OIS::KC_A )
            GameCore::mClientGraphics->mSpawnScreen->switchCar( -1 );
        else if( evt.key == OIS::KC_RIGHT || evt.key == OIS::KC_D )
            GameCore::mClientGraphics->mSpawnScreen->switchCar( 1 );
        else if( evt.key == OIS::KC_RETURN )
            GameCore::mClientGraphics->mSpawnScreen->selectCar();
    }
#endif
#ifdef COLLISION_DOMAIN_SERVER
    // Inject UP to the GUI. Has to be done here to prevent rollover as a button is
    // held down between frames, causing multiple actions from a single keypress.
    if (evt.key == OIS::KC_UP)
        GameCore::mGui->loadConsoleHistory(true);
    else if (evt.key == OIS::KC_DOWN)
        GameCore::mGui->loadConsoleHistory(false);
#endif

    return true;
}


/// @brief  Called whenever a keyboard key is released.
/// @param  evt  The KeyEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::keyReleased (const OIS::KeyEvent &evt)
{
	CEGUI::System::getSingleton().injectKeyUp(evt.key);

    return true;
}

/// @brief  Called whenever the mouse is moved.
/// @param  evt  The MouseEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::mouseMoved (const OIS::MouseEvent& evt)
{
	CEGUI::System& guiSys = CEGUI::System::getSingleton();
#ifdef COLLISION_DOMAIN_CLIENT
	guiSys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	// Scroll wheel.
	if (evt.state.Z.rel)
		guiSys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
#else
    // Inject mouse movements into the GUI
    guiSys.injectMousePosition(evt.state.X.abs, evt.state.Y.abs);
	if (evt.state.Z.rel)
		guiSys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);

    // Check if the mouse is within the window
    if (evt.state.X.abs <= 0 || evt.state.X.abs >= evt.state.width ||
        evt.state.Y.abs <= 0 || evt.state.Y.abs >= evt.state.height)
    {
        CEGUI::MouseCursor::getSingleton().hide();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        ShowCursor(true);
#else
        #error "Currently no non-windows method has been implemented to hide the hardware cursor."
#endif
    }
    else
    {
	    CEGUI::MouseCursor::getSingleton().show();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        ShowCursor(false);
#else
        #error "Currently no non-windows method has been implemented to hide the hardware cursor."
#endif
    }
#endif
    return true;
}


/// @brief  Called whenever a mouse button is pressed.
/// @param  evt  The MouseEvent associated with this call.
/// @param  id   The mouse button that was pressed.
/// @return Whether the event has been serviced.
bool Input::mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    // Play the car horn on left or right button press
#ifdef COLLISION_DOMAIN_CLIENTo
    Player*      localPlayer = GameCore::mPlayerPool->getLocalPlayer();
    SpawnScreen* spawnScreen = GameCore::mClientGraphics->mSpawnScreen;
    
    if (!spawnScreen && localPlayer && localPlayer->getCar())
        localPlayer->getCar()->playCarHorn();
#endif

	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}


/// @brief  Called whenever a mouse button is released.
/// @param  evt  The MouseEvent associated with this call.
/// @param  id   The mouse button that was released.
/// @return Whether the event has been serviced.
bool Input::mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
    return true;
}

CEGUI::MouseButton Input::convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}