/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;

/*  Width:    2490mm (not inc wing mirrors, 2866mm with them)
    Height:   2816mm (inc wheels, cab alone is 2589mm)
    Length:   5725mm
    Wheelbase:    3526mm
    Weight:    7396kg
    Engine:    12 litre, 420bhp                               */
#define TRUCK_WIDTH_NO_WING_MIRRORS 2.490f
#define TRUCK_HEIGHT_CAB_ONLY       2.589f
#define TRUCK_HEIGHT_INC_WHEELS     2.816f
#define TRUCK_LENGTH                5.725f
#define TRUCK_WHEELBASE             3.526f
#define TRUCK_LENGTH_SHIFT_Z         -0.5f

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void TruckCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =  20.0f;
    mSuspensionDamping      =   2.3f;
    mSuspensionCompression  =   4.4f;
    mRollInfluence          =   0.1f;//1.0f;
    mSuspensionRestLength   =   0.6f;
    mMaxSuspensionTravelCm  = 500.0f;
    mFrictionSlip           =  10.5f;

    mWheelRadius      =  0.361902462f;
    mWheelWidth       =  0.1349448267f;
    mWheelFriction    = 1e30f;//1000;//1e30f;
    mConnectionHeight =  0.7f;
    
    mSteerIncrement = 0.015f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 8000.0f;
    mMaxBrakeForce = 10000.0f;
}

/// @brief  Constructor to create a car, add its graphical model to ogre and add its physics model to bullet.
/// @param  sceneMgr     The Ogre graphics world.
/// @param  world        The bullet physics world.
/// @param  uniqueCarID  A unique ID for the car so that generated nodes do not have (forbidden) name collisions.
TruckCar::TruckCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    Ogre::Vector3 chassisShift(0, 0.70f, 0.0f);

    initTuning();
    initNodes();
    initGraphics();
    initBody(carPosition, chassisShift);
    initWheels();
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
TruckCar::~TruckCar(void)
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


/// @brief  Initialises the node tree for this car.
void TruckCar::initNodes()
{
    mPlayerNode  = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mLDoorNode   = mBodyNode->createChildSceneNode("LDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRDoorNode   = mBodyNode->createChildSceneNode("RDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mLWingmirrorNode = mBodyNode->createChildSceneNode("LWingmirrorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRWingmirrorNode = mBodyNode->createChildSceneNode("RWingmirrorNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void TruckCar::initGraphics()
{
    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    createGeometry("CarBody", "truck_body.mesh", "truck_body_uv", mChassisNode);
    PhysicsCore::auto_scale_scenenode(mChassisNode);
   // mChassisNode->setPosition(chassisShift); - Doesn't work well with this mesh!!!

    // load the left door baby
    createGeometry("CarEntity_LDoor", "truck_ldoor.mesh", "truck_door_uv", mLDoorNode);
    PhysicsCore::auto_scale_scenenode(mLDoorNode);
    //mLDoorNode->translate(1.0 * 0.019, 20.0 * 0.019, 22.0 * 0.019);
    
    // lets get a tasty right door
    createGeometry("CarEntity_RDoor", "truck_rdoor.mesh", "truck_door_uv", mRDoorNode);
    PhysicsCore::auto_scale_scenenode(mRDoorNode);
    //mRDoorNode->translate(-46.0 * 0.019, 20.0 * 0.019, 22.0 * 0.019);

    // and now a regular rear bumper
    createGeometry("CarEntity_RBumper", "truck_rbumper.mesh", "truck_bumper_uv", mRBumperNode);
    mRBumperNode->scale(-1, 1, 1);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);
   // mRBumperNode->translate(0, 20.0 * 0.019, -135.0 * 0.019);

    // Wingmirrors
    createGeometry("CarEntity_LWingmirror", "truck_lwingmirror.mesh", "truck_wingmirror_mirror", mLWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mLWingmirrorNode);
    createGeometry("CarEntity_RWingmirror", "truck_rwingmirror.mesh", "truck_wingmirror_mirror", mRWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mRWingmirrorNode);

    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "truck_wheel.mesh", "truck_wheel_uv", mFLWheelNode);
    //mFLWheelNode->scale(-1, 1, 1);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "truck_wheel.mesh", "truck_wheel_uv", mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "truck_wheel.mesh", "truck_wheel_uv", mRLWheelNode);
    //mRLWheelNode->scale(-1, 1, 1);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "truck_wheel.mesh", "truck_wheel_uv", mRRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
    
    //Ogre::Entity *entity = mSceneMgr->createEntity("fag","car2_wheel.mesh");
    //const Ogre::AxisAlignedBox boundingBox = entity->getBoundingBox();
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void TruckCar::initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift)
{
    // shift chassis collisionbox up chassisShift units above origin
	
    compoundChassisShape = new OgreBulletCollisions::CompoundCollisionShape();
    
    chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(TRUCK_WIDTH_NO_WING_MIRRORS, TRUCK_HEIGHT_CAB_ONLY, 5.72f));
    compoundChassisShape->addChildShape(chassisShape, Vector3(0, TRUCK_HEIGHT_CAB_ONLY + (TRUCK_HEIGHT_INC_WHEELS - TRUCK_HEIGHT_CAB_ONLY), TRUCK_LENGTH_SHIFT_Z));

    OgreBulletCollisions::BoxCollisionShape *chassisShapeTop = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.764775f, 0.33f, 1.06672f));
    compoundChassisShape->addChildShape(chassisShapeTop, Ogre::Vector3(0.0f, 2.3f, -0.25f));

    OgreBulletCollisions::BoxCollisionShape *chassisShapeAntiRoll = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(0.01f, 0.15f, 0.01f));
    compoundChassisShape->addChildShape(chassisShapeAntiRoll, Ogre::Vector3(0.0f, 1.63f, 0.0f));


    
    // name given here needs to be unique to have more than one in the scene
    //mCarChassis = new OgreBulletDynamics::WheeledRigidBody("CarRigidBody" + boost::lexical_cast<std::string>(mUniqueCarID), mWorld);
    
    mCarChassis = (OgreBulletDynamics::WheeledRigidBody*) (
        new FuckOgreBulletWheeledRigidBody(
            "CarRigidBody" + boost::lexical_cast<std::string>(mUniqueCarID),
            mWorld,
            COL_CAR,
            COL_CAR | COL_ARENA | COL_POWERUP));
    
    // attach physics shell to mBodyNode
    mCarChassis->setShape (mBodyNode, compoundChassisShape, 0.6f, 0.6f, 800, carPosition, Ogre::Quaternion::IDENTITY);
    mCarChassis->setDamping(0.2f, 0.2f);

    mCarChassis->disableDeactivation();
    mTuning = new OgreBulletDynamics::VehicleTuning(
        mSuspensionStiffness, mSuspensionCompression, mSuspensionDamping, mMaxSuspensionTravelCm, mFrictionSlip);

    mVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
    
    mVehicle = new OgreBulletDynamics::RaycastVehicle(mCarChassis, mTuning, mVehicleRayCaster);
    
    // This line is needed otherwise the model appears wrongly rotated.
    mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex
    
    mbtRigidBody = mCarChassis->getBulletRigidBody();
}


/// @brief  Attaches 4 wheels to the car chassis.
void TruckCar::initWheels()
{
       /* #define TRUCK_WIDTH_NO_WING_MIRRORS 2.490f;
#define TRUCK_HEIGHT_CAB_ONLY       2.589f;
#define TRUCK_HEIGHT_INC_WHEELS     2.816f;
#define TRUCK_LENGTH                5.725f;
#define TRUCK_WHEELBASE             3.526f;*/

    float wheelBase = 3.526;

    Ogre::Vector3 wheelDirectionCS0(0,-1,0);
    Ogre::Vector3 wheelAxleCS(-1,0,0);

    #define CUBE_HALF_EXTENTS 1
    bool isFrontWheel = true;
    
    // Wheel 1 - Front Left
    //Ogre::Vector3 connectionPointCS0 (CUBE_HALF_EXTENTS-(0.3*mWheelWidth), mConnectionHeight, (2*CUBE_HALF_EXTENTS-mWheelRadius) +0.2);
    Ogre::Vector3 connectionPointCS0 (wheelBase-(mWheelWidth), mConnectionHeight, wheelBase + (-mWheelRadius));
    mVehicle->addWheel(mFLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
    
    // Wheel 2 - Front Right
    connectionPointCS0 = Ogre::Vector3(-CUBE_HALF_EXTENTS+(0.3*mWheelWidth), mConnectionHeight, (2*CUBE_HALF_EXTENTS-mWheelRadius) + 0.2);
    mVehicle->addWheel(mFRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
                    
    isFrontWheel = false;

    // Wheel 3 - Rear Right
    connectionPointCS0 = Ogre::Vector3(-CUBE_HALF_EXTENTS+(0.3*mWheelWidth), mConnectionHeight, (-2*CUBE_HALF_EXTENTS+mWheelRadius) +0.2);
    mVehicle->addWheel(mRRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);

    // Wheel 4 - Rear Left
    connectionPointCS0 = Ogre::Vector3(CUBE_HALF_EXTENTS-(0.3*mWheelWidth), mConnectionHeight, (-2*CUBE_HALF_EXTENTS+mWheelRadius) +0.2);
    mVehicle->addWheel(mRLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
}

