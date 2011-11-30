/**
 * @file	Input.cpp
 * @brief 	Controls the users input.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Input.h"



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
void Input::destroyInputSystem (void)
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
void Input::capture (void)
{
    mKeyboard->capture();
    mMouse->capture();
}


/// @brief  Constructs an InputState object based on the key state (at the time of the last Input::capture()).
/// @return The InputState object containing movement key information at the time of the previous sample.
InputState Input::getInputState (void)
{
    return InputState(mKeyboard->isKeyDown(OIS::KC_W),
                      mKeyboard->isKeyDown(OIS::KC_S),
                      mKeyboard->isKeyDown(OIS::KC_A),
                      mKeyboard->isKeyDown(OIS::KC_D) );
}


/// @brief  Called whenever a keyboard key is pressed.
/// @param  evt  The KeyEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::keyPressed (const OIS::KeyEvent &evt)
{
    return true;
}


/// @brief  Called whenever a keyboard key is released.
/// @param  evt  The KeyEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::keyReleased (const OIS::KeyEvent &evt)
{
    return true;
}



/// @brief  Called whenever the mouse is moved.
/// @param  evt  The MouseEvent associated with this call.
/// @return Whether the event has been serviced.
bool Input::mouseMoved (const OIS::MouseEvent& evt)
{
    return true;
}


/// @brief  Called whenever a mouse button is pressed.
/// @param  evt  The MouseEvent associated with this call.
/// @param  id   The mouse button that was pressed.
/// @return Whether the event has been serviced.
bool Input::mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    return true;
}


/// @brief  Called whenever a mouse button is released.
/// @param  evt  The MouseEvent associated with this call.
/// @param  id   The mouse button that was released.
/// @return Whether the event has been serviced.
bool Input::mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
    return true;
}