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
    SimpleCoupeCar(int uniqueCarID, CarSkin skin);
    virtual ~SimpleCoupeCar(void);
    void playCarHorn();
    virtual void updateTeam (int teamNumber);
    virtual void loadDestroyedModel (void);

private:
    void initTuning();
    void initNodes();
    void initGraphics(btTransform& chassisShift, CarSkin skin);
    void initBody(Ogre::Vector3 carPosition, btTransform& chassisShift);
    void initWheels();
    
    // Car related Nodes (initNodes())
    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mFLDoorNode;
    Ogre::SceneNode *mFRDoorNode;
    Ogre::SceneNode *mRLDoorNode;
    Ogre::SceneNode *mRRDoorNode;
    Ogre::SceneNode *mFBumperNode;
    Ogre::SceneNode *mRBumperNode;

    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;
};

#endif // #ifndef __SimpleCoupeCar_h_

