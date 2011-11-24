/*------------------------------------------------------------------------------
  File:     GraphicsApplication.h
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.h).
 ------------------------------------------------------------------------------*/
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "stdafx.h"
#include "GraphicsCore.h"
#include "../../base/includes/player.h"
#include "../../networking/includes/fakeServer.h"



/******************** CLASS DEFINITIONS ********************/
class GraphicsApplication : public GraphicsCore
{
public:
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

protected:
    virtual void createScene(void);
    virtual void createFrameListener(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    //virtual bool keyPressed( const OIS::KeyEvent& evt );
    //virtual bool keyReleased( const OIS::KeyEvent& evt );
    virtual bool mouseMoved( const OIS::MouseEvent& evt );
    virtual bool mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent& evt, OIS::MouseButtonID id );

private:
    void setupLighting (void);
    void setupArena (void);
    void setupNetworking (void);
    
    fakeServer server;
    int clientID;
    player clientPlayerList[1]; // which is actually an array until I can figure out c++ lists
    //boost::ptr_list<player> playerList;
    //boost::ptr_list<player>::iterator playerListItr;
	//Ogre::SceneNode *mPlayerNode;   // The SceneNode the camera is currently attached to
	//Ogre::Real carTurningConstant;          // The rotate constant
    //Ogre::Real carAccelerationConstant;
    //Ogre::Real carTopSpeedConstant;
    //Ogre::Real carFrictionConstant;
	//Ogre::Real carSpeed;            // The movement constant
	//Ogre::Real carAcceleration;     // Value to move in the correct direction
    //Ogre::Degree carRotation;
};

#endif // #ifndef GRAPHICSAPPLICATION_H
