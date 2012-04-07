/**
 * @file	SimpleCoupeCar.h
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#ifndef __TRUCKCAR_H
#define __TRUCKCAR_H

#include "stdafx.h"
#include "SharedIncludes.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#include "boost\lexical_cast.hpp"
#else
	#include "boost/lexical_cast.hpp"
#endif

class TruckCar : public Car
{
public:
    TruckCar(int uniqueCarID, CarSkin skin, bool silentCar = false);
    virtual ~TruckCar(void);
    void frameEvent();
    void playCarHorn();
    virtual void updateTeam (int teamNumber);
    virtual void loadDestroyedModel (void);

    virtual void makeBitsFallOff();

private:
    void initTuning();
    void initNodes();
    void initGraphics();
    void initBody(Ogre::Vector3 carPosition, btTransform& chassisShift);
    void initDoors( btTransform& chassisShift );
    void initWheels();
    
    // Car related Nodes (initNodes())
    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mLDoorNode;
    Ogre::SceneNode *mRDoorNode;
    Ogre::SceneNode *mRBumperNode;
    Ogre::SceneNode *mLWingmirrorNode;
    Ogre::SceneNode *mRWingmirrorNode;

    btRigidBody     *mLDoorBody;
    btRigidBody     *mRDoorBody;
    btRigidBody     *mRBumperBody;
    btRigidBody     *mLWingmirrorBody;
    btRigidBody     *mRWingmirrorBody;

    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;
    
    OgreOggISound *mHornSound;
    OgreOggISound *mEngineSound;
};

#endif // #ifndef __TRUCKCAR_H

