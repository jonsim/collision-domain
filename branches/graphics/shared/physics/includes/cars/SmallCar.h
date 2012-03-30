/**
 * @file	SimpleCoupeCar.h
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#ifndef __SMALLCAR_H_
#define __SMALLCAR_H_

#include "stdafx.h"
#include "SharedIncludes.h"
#include "boost\lexical_cast.hpp"

class SmallCar : public Car
{
public:
    SmallCar(int uniqueCarID, CarSkin skin);
    virtual ~SmallCar(void);
    void playCarHorn();
    virtual void updateTeam (int teamNumber);
    virtual void loadDestroyedModel (void);

    virtual void makeBitsFallOff();

private:
    void initTuning();
    void initNodes();
    void initGraphics(btTransform& chassisShift);
    void initBody(Ogre::Vector3 carPosition, btTransform& chassisShift);
    void initWheels();
    
    // Car related Nodes (initNodes())
    Ogre::SceneNode *mLDoorNode;
    Ogre::SceneNode *mRDoorNode;
    Ogre::SceneNode *mFBumperNode;
    Ogre::SceneNode *mRBumperNode;
    Ogre::SceneNode *mLHeadlightNode;
    Ogre::SceneNode *mRHeadlightNode;

    btRigidBody     *mLDoorBody;
    btRigidBody     *mRDoorBody;
    btRigidBody     *mFBumperBody;
    btRigidBody     *mRBumperBody;
    btRigidBody     *mLHeadlightBody;
    btRigidBody     *mRHeadlightBody;
    
    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;
};

#endif // #ifndef __SMALLCAR_H_

