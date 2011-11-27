/**
 * @file	GraphicsApplication.h
 * @brief 	Adds objects to the graphics interface.
 */
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GraphicsCore.h"
#include "Player.h"
#include "Frame.h"
#include "PlayerState.h"
#include "InputState.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief  Adds objects to the graphics interface.
 *
 *          Derived from the Ogre Tutorial Framework (TutorialApplication.h).
 */
class GraphicsApplication : public GraphicsCore
{
public:
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

protected:
    virtual void createScene(void);
    virtual void createFrameListener(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool mouseMoved( const OIS::MouseEvent& evt );
    virtual bool mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent& evt, OIS::MouseButtonID id );

private:
    void setupLighting (void);
    void setupArena (void);
    void setupNetworking (void);
    
    int clientID;               ///< The client ID which is assigned by the server.
    Player clientPlayerList[1]; ///< An array of all players in the game to keep track of.
};

#endif // #ifndef GRAPHICSAPPLICATION_H
