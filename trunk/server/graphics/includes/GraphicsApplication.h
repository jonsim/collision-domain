/**
 * @file	GraphicsApplication.h
 * @brief 	Adds objects to the graphics interface and contains the framelistener
 */
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


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

    int clientID;               ///< The client ID which is assigned by the server.

    //Player players[1]; ///< An array of all players in the game to keep track of.
    btClock *mClock;
    bool firstFrameOccurred;
	Ogre::Entity* getArenaEntity();
protected:
	CEGUI::OgreRenderer* mGuiRenderer;

    virtual void createScene(void);
    virtual void createFrameListener(void);
	
    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool frameStarted(const Ogre::FrameEvent& evt);
    virtual bool frameEnded(const Ogre::FrameEvent& evt);

private:
    void setupLighting (uint8_t mode);
    void setupArena (void);
    void setupNetworking (void);
	Ogre::Entity* arenaEntity; //Needed to do calculations on arena size
};

#endif // #ifndef GRAPHICSAPPLICATION_H
