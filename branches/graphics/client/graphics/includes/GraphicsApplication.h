/**
 * @file    GraphicsApplication.h
 * @brief     Adds objects to the graphics interface and contains the framelistener
 */
#ifndef GRAPHICSAPPLICATION_H
#define GRAPHICSAPPLICATION_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief  Adds objects to the graphics interface.
 */
class GraphicsCore;
class GraphicsApplication : public GraphicsCore
{
public:
    GraphicsApplication (void);
    virtual ~GraphicsApplication (void);
    
    void startBenchmark (uint8_t stage);

protected:
    virtual void createCamera (void);
    virtual void createViewports (void);
    virtual void createScene (void);
    virtual void createFrameListener (void);
    
    // Ogre::FrameListener overrides.
    virtual bool frameRenderingQueued (const Ogre::FrameEvent& evt);
    virtual bool frameStarted (const Ogre::FrameEvent& evt);
    virtual bool frameEnded (const Ogre::FrameEvent& evt);

private:
    // Setup functions.
    virtual void setupGUI (void);

    void finishBenchmark (uint8_t stage, float averageTriangles);

    bool    mBenchmarkRunning;
    uint8_t mBenchmarkStage;
};

#endif // #ifndef GRAPHICSAPPLICATION_H
