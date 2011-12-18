

#include "stdafx.h"
#include "Car.h"
#include "boost\lexical_cast.hpp"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

Car::~Car(void)
{
    // Cleanup Bodies:
    delete mVehicle;
    delete mVehicleRayCaster;
    delete mTuning;
    delete mCarChassis;

    // Cleanup Shapes:
    delete compoundChassisShape;
    delete chassisShape;
}

Car::Car(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    initTuning();
    initNodes();
    initGraphics();
    initBody();
    initWheels();
}


void Car::initTuning()
{
    mSuspensionStiffness    =  20.0f;
    mSuspensionDamping      =   2.3f;
    mSuspensionCompression  =   4.4f;
    mRollInfluence          =   0.1f;//1.0f;
    mSuspensionRestLength   =   0.6f;
    mMaxSuspensionTravelCm  = 500.0f;
    mFrictionSlip           =  10.5f;

    mWheelRadius      =  0.5f;
    mWheelWidth       =  0.4f;
    mWheelFriction    = 1e30f;//1000;//1e30f;
    mConnectionHeight =  0.7f;
}


void Car::initNodes()
{
    mPlayerNode  = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mLDoorNode   = mBodyNode->createChildSceneNode("LDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRDoorNode   = mBodyNode->createChildSceneNode("RDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


void Car::initGraphics()
{
    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    createGeometry("CarBody", "chassis.mesh", "car2_body", mChassisNode); // "car2_body.mesh"
    Ogre::Vector3 chassisShift(0, 1.0, 0);
    mChassisNode->setPosition(chassisShift);

    //mBodyNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));

    // load the left door baby
    //createGeometry("CarEntity_LDoor", "car2_door.mesh", "car2_door", mLDoorNode);
    //mLDoorNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mLDoorNode->translate(43, 20, 22);
    
    // lets get a tasty right door
    //createGeometry("CarEntity_RDoor", "car2_door.mesh", "car2_door", mRDoorNode);
    //mRDoorNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mRDoorNode->scale(-1, 1, 1);
    //mRDoorNode->translate(-46, 20, 22);

    // and now a sweet sweet front bumper
    //createGeometry("CarEntity_FBumper", "car2_Fbumper.mesh", "car2_Fbumper", mFBumperNode);
    //mFBumperNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mFBumperNode->translate(0, 20, 140);

    // and now a regular rear bumper
    //createGeometry("CarEntity_RBumper", "car2_Rbumper.mesh", "car2_Rbumper", mRBumperNode);
    //mRBumperNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mRBumperNode->translate(0, 20, -135);

    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "wheel.mesh", "car2_wheel", mFLWheelNode); // "car2_wheel.mesh"
    //mFLWheelNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mFLWheelNode->scale(-1, 1, 1);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "wheel.mesh", "car2_wheel", mFRWheelNode); // "car2_wheel.mesh"
    //mFRWheelNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "wheel.mesh", "car2_wheel", mRLWheelNode); // "car2_wheel.mesh"
    //mRLWheelNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));
    //mRLWheelNode->scale(-1, 1, 1);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "wheel.mesh", "car2_wheel", mRRWheelNode); // "car2_wheel.mesh"
    //mRRWheelNode->scale(Ogre::Real(0.05), Ogre::Real(0.05), Ogre::Real(0.05));

}


void Car::initBody()
{
    //Ogre::Vector3 carPosition;
    //Ogre::Vector3 chassisShift;

    const Ogre::Vector3 carPosition(15, 3, -15);
    const Ogre::Vector3 chassisShift(0, 1.0, 0); // shift chassis collisionbox up 50 units above origin

    chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1.0f, 0.75f, 2.1f));//Ogre::Vector3(1.f,0.75f,2.1f));

    compoundChassisShape = new OgreBulletCollisions::CompoundCollisionShape();
    compoundChassisShape->addChildShape(chassisShape, chassisShift);
    
    // name given here needs to be unique to have more than one in the scene
    mCarChassis = new OgreBulletDynamics::WheeledRigidBody("CarRigidBody" + boost::lexical_cast<std::string>(mUniqueCarID), mWorld);

    // attach physics shell to mBodyNode
    mCarChassis->setShape (mBodyNode, compoundChassisShape, 0.6f, 0.6f, 800, carPosition, Ogre::Quaternion::IDENTITY);
    mCarChassis->setDamping(0.2, 0.2);

    mCarChassis->disableDeactivation ();
    mTuning = new OgreBulletDynamics::VehicleTuning(
        mSuspensionStiffness, mSuspensionCompression, mSuspensionDamping, mMaxSuspensionTravelCm, mFrictionSlip);

    mVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
    mVehicle = new OgreBulletDynamics::RaycastVehicle(mCarChassis, mTuning, mVehicleRayCaster);
    
    // This line is needed otherwise the model appears wrongly rotated.
    mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex
}


void Car::initWheels()
{
    Ogre::Vector3 wheelDirectionCS0(0,-1,0);
    Ogre::Vector3 wheelAxleCS(-1,0,0);

    #define CUBE_HALF_EXTENTS 1
    bool isFrontWheel = true;

    // Wheel 1 - Front Left
    Ogre::Vector3 connectionPointCS0 (CUBE_HALF_EXTENTS-(0.3*mWheelWidth), mConnectionHeight, 2*CUBE_HALF_EXTENTS-mWheelRadius);
    mVehicle->addWheel(mFLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);

    // Wheel 2 - Front Right
    connectionPointCS0 = Ogre::Vector3(-CUBE_HALF_EXTENTS+(0.3*mWheelWidth), mConnectionHeight, 2*CUBE_HALF_EXTENTS-mWheelRadius);
    mVehicle->addWheel(mFRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
                    
    isFrontWheel = false;

    // Wheel 3 - Rear Right
    connectionPointCS0 = Ogre::Vector3(-CUBE_HALF_EXTENTS+(0.3*mWheelWidth), mConnectionHeight, -2*CUBE_HALF_EXTENTS+mWheelRadius);
    mVehicle->addWheel(mRRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);

    // Wheel 4 - Rear Left
    connectionPointCS0 = Ogre::Vector3(CUBE_HALF_EXTENTS-(0.3*mWheelWidth), mConnectionHeight, -2*CUBE_HALF_EXTENTS+mWheelRadius);
    mVehicle->addWheel(mRLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
}


/// @brief  Called once a frame as the CPU has finished its calculations and the GPU is about to start rendering.
/// @param  evt  The FrameEvent associated with this frame's rendering.
/// @return Whether the application should continue (i.e.\ false will force a shut down).
Ogre::SceneNode *Car::attachCamNode()
{
    if (mCamNode != NULL) return mCamNode;

    // else we need to make a new camera
    mCamArmNode = mBodyNode->createChildSceneNode("CamArmNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mCamNode = mCamArmNode->createChildSceneNode("CamNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    return mCamNode;
}


void Car::createGeometry(const std::string &entityName,
                    const std::string &meshName,
                    const std::string &materialName,
                    Ogre::SceneNode *toAttachTo)
{
    Ogre::Entity* entity;
    entity = mSceneMgr->createEntity(entityName + boost::lexical_cast<std::string>(mUniqueCarID), meshName);
    //entity->setMaterialName(materialName);

    int GEOMETRY_QUERY_MASK = 1<<2;
    entity->setQueryFlags(GEOMETRY_QUERY_MASK); // lets raytracing hit this object (for physics)
#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    //entity->setCastShadows(true);
    toAttachTo->attachObject(entity);
}

