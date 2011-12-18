
#ifndef __Car_h_
#define __Car_h_

#include "stdafx.h"

class Car
{
public:
    Car(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID);
    virtual ~Car(void);
    Ogre::SceneNode *attachCamNode();

private:
    void initTuning();
    void initNodes();
    void initGraphics();
    void initBody();
    void initWheels();
    void createGeometry(const std::string &entityName,
                        const std::string &meshName,
                        const std::string &materialName,
                        Ogre::SceneNode *toAttachTo);
    
    // Data for whole class
    int mUniqueCarID;
    Ogre::SceneManager* mSceneMgr; // Graphics object
    OgreBulletDynamics::DynamicsWorld *mWorld; // Collisions object
    
    // Camera node (if not null its attached to bodyNode) (attachCamNode())
    Ogre::SceneNode *mCamArmNode;
    Ogre::SceneNode *mCamNode;

    // Car related Nodes (initNodes())
    Ogre::SceneNode *mPlayerNode;
    
    Ogre::SceneNode *mBodyNode;
    Ogre::SceneNode *mWheelsNode;

    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mLDoorNode;
    Ogre::SceneNode *mRDoorNode;
    Ogre::SceneNode *mFBumperNode;
    Ogre::SceneNode *mRBumperNode;

    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;

    // mTuning properties (initTuning())
    float mSuspensionStiffness;
    float mSuspensionDamping;
    float mSuspensionCompression;
    float mRollInfluence;
    float mSuspensionRestLength;
    float mMaxSuspensionTravelCm;
    float mFrictionSlip;

    float mWheelRadius;
    float mWheelWidth;
    float mWheelFriction;
    float mConnectionHeight;

    // Car physics objects (initBody())
    OgreBulletCollisions::BoxCollisionShape      *chassisShape;
    OgreBulletCollisions::CompoundCollisionShape *compoundChassisShape;
    OgreBulletDynamics::WheeledRigidBody         *mCarChassis;
    OgreBulletDynamics::VehicleTuning            *mTuning;
    OgreBulletDynamics::VehicleRayCaster         *mVehicleRayCaster;
    OgreBulletDynamics::RaycastVehicle           *mVehicle;
};

#endif // #ifndef __Car_h_

