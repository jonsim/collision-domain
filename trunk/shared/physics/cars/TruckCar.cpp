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
    Wheelbase:    3526mm (this class uses 3575 as its 5cm off)
    Weight:    7396kg
    Engine:    12 litre, 420bhp
    Tyre Diameter: 1046mm
    Tyre Width: 243mm (bit that touches ground, not bounding box)
*/

#define CRITICAL_DAMPING_COEF       0.1f

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void TruckCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    mBrakingForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =   200.0f;
    mSuspensionDamping      =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness);
    mSuspensionCompression  =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness) + 0.2;
    mRollInfluence          =   0.1f;
    mSuspensionRestLength   =   0.4f;
    mMaxSuspensionTravelCm  =   26.0f;
    mFrictionSlip           =   3.0f;
	mChassisLinearDamping   =   0.2f;
	mChassisAngularDamping  =   0.2f;
	mChassisRestitution		=   0.6f;
	mChassisFriction        =   0.6f;
	mChassisMass            =   7396.0f;

    mWheelRadius      =  1.046f; // this is actually diameter!!
    mWheelWidth       =  0.243f;
    mWheelFriction    = 1e30f;//1000;//1e30f;
    mConnectionHeight =  1.2f; // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement = 0.015f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 10000.0f;
    mMaxBrakeForce = 300.0f;

	mFrontWheelDrive = true;
	mRearWheelDrive  = true;
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
    Ogre::Vector3 chassisShift(0, 1.6f, 3.2f);

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


void TruckCar::playCarHorn()
{
    GameCore::mAudioCore->playCarHorn(HORN_LOW);
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
	
	// setup particles. This needs to be propogated.
    mExhaustSystem = mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "Truck/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);

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
    PhysicsCore::auto_scale_scenenode(mRBumperNode);
   // mRBumperNode->translate(0, 20.0 * 0.019, -135.0 * 0.019);

    // Wingmirrors
    createGeometry("CarEntity_LWingmirror", "truck_lwingmirror.mesh", "truck_wingmirror_mirror", mLWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mLWingmirrorNode);
    createGeometry("CarEntity_RWingmirror", "truck_rwingmirror.mesh", "truck_wingmirror_mirror", mRWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mRWingmirrorNode);

    
    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "truck_lwheel.mesh", "truck_wheel_uv", mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "truck_rwheel.mesh", "truck_wheel_uv", mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "truck_lwheel.mesh", "truck_wheel_uv", mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "truck_rwheel.mesh", "truck_wheel_uv", mRRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
    

    // ONLY WHILE USING WHEEL.MESH, UNTIL OUR WHEEL MESHES ARE FIXED
    /*createGeometry("CarEntity_FLWheel", "wheel.mesh", mFLWheelNode);
    createGeometry("CarEntity_FRWheel", "wheel.mesh", mFRWheelNode);
    createGeometry("CarEntity_RLWheel", "wheel.mesh", mRLWheelNode);
    createGeometry("CarEntity_RRWheel", "wheel.mesh", mRRWheelNode);
    float scale = 2.1;
    mFLWheelNode->scale(scale, scale, scale);
    mFRWheelNode->scale(scale, scale, scale);
    mRLWheelNode->scale(scale, scale, scale);
    mRRWheelNode->scale(scale, scale, scale);*/
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void TruckCar::initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = mSceneMgr->createEntity("SmallCarCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "truck_collision.mesh");
    entity->setDebugDisplayEnabled( false );
    compoundChassisShape = new OgreBulletCollisions::CompoundCollisionShape();

    // Transformation matrix to scale the imported mesh
    Ogre::Matrix4 matScale(MESH_SCALING_CONSTANT, 0, 0, 0, 0, MESH_SCALING_CONSTANT, 0, 0, 0, 0, MESH_SCALING_CONSTANT, 0, 0, 0, 0, 1.0);

    // Create a compound shape from the mesh's vertices
    OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = 
        new OgreBulletCollisions::StaticMeshToShapeConverter(entity, matScale);

    OgreBulletCollisions::CompoundCollisionShape *tmp = trimeshConverter->createConvexDecomposition();

    delete trimeshConverter;


    // Shift the mesh (this does work in a physical sense, but the wireframe is still drawn in the wrong place)
    compoundChassisShape->addChildShape( tmp, chassisShift );

    mCarChassis = (OgreBulletDynamics::WheeledRigidBody*) (
        new FuckOgreBulletWheeledRigidBody(
            "CarRigidBody" + boost::lexical_cast<std::string>(mUniqueCarID),
            mWorld,
            COL_CAR,
            COL_CAR | COL_ARENA | COL_POWERUP));
    
    // attach physics shell to mBodyNode
    mCarChassis->setShape(mBodyNode, compoundChassisShape, mChassisRestitution, mChassisFriction, mChassisMass, carPosition, Ogre::Quaternion::IDENTITY);
    mCarChassis->setDamping(mChassisLinearDamping, mChassisAngularDamping);

    mCarChassis->disableDeactivation();
    mTuning = new OgreBulletDynamics::VehicleTuning(
        mSuspensionStiffness, mSuspensionCompression, mSuspensionDamping, mMaxSuspensionTravelCm, mFrictionSlip);

    mVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
    
    mVehicle = new OgreBulletDynamics::RaycastVehicle(mCarChassis, mTuning, mVehicleRayCaster);
    
    // This line is needed otherwise the model appears wrongly rotated.
    mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex
    
    mbtRigidBody = mCarChassis->getBulletRigidBody();

    OgreBulletCollisions::DebugCollisionShape *dbg = mCarChassis->getDebugShape();

    Ogre::Matrix4 matChassisShift;
    matChassisShift.makeTrans( chassisShift );
    dbg->setWorldTransform( matChassisShift );
}


/// @brief  Attaches 4 wheels to the car chassis.
void TruckCar::initWheels()
{
    float wheelBaseLength = 3.576f;
    float wheelBaseHalfWidth  = 2.1f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = -1.15f;

    Ogre::Vector3 wheelDirectionCS0(0,-1,0);
    Ogre::Vector3 wheelAxleCS(-1,0,0);

    bool isFrontWheel = true;
    
    // Wheel 0 - Front Left
    Ogre::Vector3 connectionPointCS0 (wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength );
    mVehicle->addWheel(mFLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
    
    // Wheel 1 - Front Right
    connectionPointCS0 = Ogre::Vector3(-wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength );
    mVehicle->addWheel(mFRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
    
    isFrontWheel = false;

    // Wheel 2 - Rear Right
    connectionPointCS0 = Ogre::Vector3(-wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ - wheelBaseLength);
    mVehicle->addWheel(mRRWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);

    // Wheel 3 - Rear Left
    connectionPointCS0 = Ogre::Vector3(wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ - wheelBaseLength);
    mVehicle->addWheel(mRLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);
}

