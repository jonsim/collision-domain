/**
 * @file	SimpleCoupeCar.h
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#ifndef __SimpleCoupeCar_h_
#define __SimpleCoupeCar_h_

#include "stdafx.h"
#include "SharedIncludes.h"
#include "boost\lexical_cast.hpp"

class SimpleCoupeCar : Car
{
public:
    SimpleCoupeCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID);
    virtual ~SimpleCoupeCar(void);

private:
    void initTuning();
    void initNodes();
    void initGraphics(Ogre::Vector3 chassisShift);
    void initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift);
    void initWheels();
    
    // Car related Nodes (initNodes())
    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mLDoorNode;
    Ogre::SceneNode *mRDoorNode;
    Ogre::SceneNode *mFBumperNode;
    Ogre::SceneNode *mRBumperNode;

    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;
};

#endif // #ifndef __SimpleCoupeCar_h_

