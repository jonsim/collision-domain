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
    return new InputState(mKeyboard->isKeyDown(OIS::KC_W),
                          mKeyboard->isKeyDown(OIS::KC_S),
                          mKeyboard->isKeyDown(OIS::KC_A),
                          mKeyboard->isKeyDown(OIS::KC_D) );
}


/// @brief  Processes the key bound (here) to toggling widgets on screen.
/// @return Whether the key bound to toggling widgets on screen is pressed or not.
bool Input::isToggleWidget()
{
    return mKeyboard->isKeyDown(OIS::KC_G);
}

/// @brief  Processes the key bound (here) to toggling chatbox on screen.
/// @return Whether the key bound to toggling chatbox on screen is pressed or not.
bool Input::isToggleChatbox()
{
    return mKeyboard->isKeyDown(OIS::KC_T);
}

/// @brief  Processes the key bound (here) to toggling console on screen.
/// @return Whether the key bound to toggling console on screen is pressed or not.
bool Input::isToggleConsole()
{
    return mKeyboard->isKeyDown(OIS::KC_C);
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


/// @brief  Called whenever a keyboard key is pressed.
/// @param  evt  The KeyEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::keyPressed (const OIS::KeyEvent &evt)
{
	// Get the GUI system and inject the key press
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(evt.key);

	// Inject text seperately (for multi-lang keyboards)
	sys.injectChar(evt.text);

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
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	// Scroll wheel.
	if (evt.state.Z.rel)
		sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);

    return true;
}


/// @brief  Called whenever a mouse button is pressed.
/// @param  evt  The MouseEvent associated with this call.
/// @param  id   The mouse button that was pressed.
/// @return Whether the event has been serviced.
bool Input::mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
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