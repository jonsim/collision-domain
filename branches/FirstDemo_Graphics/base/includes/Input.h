/**
 * @file	Input.h
 * @brief 	Controls the user's input.
 */
#ifndef INPUT_H
#define INPUT_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InputState.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Controls the user's input using the OIS system.
 */
class Input : public OIS::KeyListener, public OIS::MouseListener
{
public:
    Input (void);
    ~Input (void);
    void createInputSystem (OIS::ParamList pl);
    void destroyInputSystem (void);
    void capture (void);
    bool keyDown (OIS::KeyCode kc);
    InputState getInputState (void);
    OIS::Mouse* getMouse (void);
    OIS::Keyboard* getKeyboard (void);

protected:
    // OIS::KeyListener
    virtual bool keyPressed (const OIS::KeyEvent &evt);
    virtual bool keyReleased (const OIS::KeyEvent &evt);
    // OIS::MouseListener
    virtual bool mouseMoved (const OIS::MouseEvent& evt);
    virtual bool mousePressed (const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    virtual bool mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID id);
    
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;
};

#endif // #ifndef INPUT_H
