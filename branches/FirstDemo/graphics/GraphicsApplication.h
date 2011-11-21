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



/******************** CLASS DEFINITIONS ********************/
class GraphicsApplication : public GraphicsCore
{
private:
    bool processUnbufferedInput(const Ogre::FrameEvent& evt);

public:
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

protected:
    virtual void createScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // #ifndef GRAPHICSAPPLICATION_H
