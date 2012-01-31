/**
 * @file	Input.h
 * @brief 	Controls the user's input.
 */
#ifndef INPUT_H
#define INPUT_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
class GraphicsCore;

/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Controls the user's input using the OIS system.
 */
class Input : public OIS::KeyListener, public OIS::MouseListener
{
    friend class GraphicsCore;

public:
    Input ();
    ~Input ();
    void createInputSystem (OIS::ParamList pl);
    void destroyInputSystem ();
    void capture ();

    // Controls are assigned here
    InputState* getInputState();
    InputState* getFreeCamInputState();
    /* bool isToggleWidget();*/
	bool isToggleChatbox();
	bool isToggleConsole();
    int getMouseXRel();
    int getMouseYRel();

protected:
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse*    mMouse;
    // OIS::KeyListener
    virtual bool keyPressed (const OIS::KeyEvent &evt);
    virtual bool keyReleased (const OIS::KeyEvent &evt);
    // OIS::MouseListener
    virtual bool mouseMoved (const OIS::MouseEvent& evt);
    virtual bool mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    virtual bool mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
};

#endif // #ifndef INPUT_H
