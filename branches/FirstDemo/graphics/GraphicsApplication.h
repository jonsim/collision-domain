/*------------------------------------------------------------------------------
  File:     GraphicsApplication.h
  Purpose:  Adds objects to the graphics interface.
            Derived from the Ogre Tutorial Framework (TutorialApplication.h).
 ------------------------------------------------------------------------------*/
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "GraphicsCore.h"



/******************** CLASS DEFINITIONS ********************/
class GraphicsApplication : public GraphicsCore
{
public:
    GraphicsApplication(void);
    virtual ~GraphicsApplication(void);

protected:
    virtual void createScene(void);
};

#endif // #ifndef GRAPHICSAPPLICATION_H
