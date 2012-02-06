/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"

using namespace Ogre;

/*
    Width:    1369mm (car not modelled with wing mirrors)
    Height:    1405mm (inc wheels, body alone is 1171mm)
    Length:    3215mm
    Wheelbase:    2049mm
    Weight:    585kg
    Engine:    767 cc, 29bhp
    Tyre Diameter: 555mm
    Tyre Width: 153mm (bit that touches ground, not bounding box)
*/

#define CRITICAL_DAMPING_COEF       0.3f

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void SmallCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    mBrakingForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =  80.0f;
    mSuspensionDamping      =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness);
    mSuspensionCompression  =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness) + 0.2;
    mRollInfluence          =   0.2f;
    mSuspensionRestLength   =   0.2f;
    mMaxSuspensionTravelCm  =   10.0f;
    mFrictionSlip           =   4.0f;
	mChassisLinearDamping   =   0.2f;
	mChassisAngularDamping  =   0.2f;
	mChassisRestitution		=   0.6f;
	mChassisFriction        =   0.6f;
	mChassisMass            = 585.0f;

    mWheelRadius            =  0.2775f; // this is actually diameter!!
    mWheelWidth             =  0.153f;
    mWheelFriction          =  1.8f;    //1000;//1e30f;
    mConnectionHeight       =  0.3f;    // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement         =  0.015f;
    mSteerToZeroIncrement   =  0.05f;   // when no input is given steer back to 0
    mSteerClamp             =  0.75f;

    mMaxAccelForce = 4000.0f;
    mMaxBrakeForce = 300.0f;

	mFrontWheelDrive = true;
	mRearWheelDrive  = false;

    mGearCount = 6;
    mCurrentGear = 1;
    mGearRatio[0] = 3.31f;
    mGearRatio[1] = 2.13f;
    mGearRatio[2] = 1.48f;
    mGearRatio[3] = 1.14f;
    mGearRatio[4] = 0.95f;
    mGearRatio[5] = 0.82f;
    mGearRatio[6] = 0.00f;
    mGearRatio[7] = 0.00f;
    mGearRatio[8] = 0.00f;
    mReverseRatio = 3.23f;
    mFinalDriveRatio = 3.45f;

    //http://www.mini2.com/forum/second-generation-mini-cooper-s/186193-6th-gear-ratio.html

    mRevTick  = 2000;
    mRevLimit = 7000;

    readTuning( "spec_smallcar.txt" );
}


/// @brief  Constructor to create a car, add its graphical model to ogre and add its physics model to bullet.
/// @param  sceneMgr     The Ogre graphics world.
/// @param  world        The bullet physics world.
/// @param  uniqueCarID  A unique ID for the car so that generated nodes do not have (forbidden) name collisions.
SmallCar::SmallCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    Ogre::Vector3 chassisShift(0, 0.55f, 0.0f);

    initTuning();
    initNodes();
    initGraphics(chassisShift);
    initBody(carPosition, chassisShift);
    initWheels();

    WheelFrictionConstraint *fricConst = new WheelFrictionConstraint( mVehicle, mbtRigidBody );
    GameCore::mPhysicsCore->mWorld->getBulletDynamicsWorld()->addConstraint( fricConst );
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
SmallCar::~SmallCar(void)
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


void SmallCar::playCarHorn()
{
    //GameCore::mAudioCore->playCarHorn(HORN_HIGH);
}


/// @brief  Initialises the node tree for this car.
void SmallCar::initNodes()
{
    mPlayerNode  = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mLDoorNode   = mBodyNode->createChildSceneNode("LDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRDoorNode   = mBodyNode->createChildSceneNode("RDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mLHeadlightNode = mBodyNode->createChildSceneNode("mLHeadlightNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRHeadlightNode = mBodyNode->createChildSceneNode("mRHeadlightNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));

	// setup particles. This needs to be propogated.
    mExhaustSystem = mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "SmallCar/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void SmallCar::initGraphics(Ogre::Vector3 chassisShift)
{
    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    createGeometry("CarBody", "small_car_body.mesh", "small_car_body_uv", mChassisNode);
    PhysicsCore::auto_scale_scenenode(mChassisNode);

    // load the left door baby
    createGeometry("CarEntity_LDoor", "small_car_ldoor.mesh", "small_car_door_uv", mLDoorNode);
    PhysicsCore::auto_scale_scenenode(mLDoorNode);
    
    // lets get a tasty right door
    createGeometry("CarEntity_RDoor", "small_car_rdoor.mesh", "small_car_door_uv", mRDoorNode);
    PhysicsCore::auto_scale_scenenode(mRDoorNode);

    // and now a sweet sweet front bumper
    createGeometry("CarEntity_FBumper", "small_car_fbumper.mesh", "small_car_bumper", mFBumperNode);
    PhysicsCore::auto_scale_scenenode(mFBumperNode);

    // and now a regular rear bumper
    createGeometry("CarEntity_RBumper", "small_car_rbumper.mesh", "small_car_bumper", mRBumperNode);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);

    // Headlights
    createGeometry("CarEntity_LHeadlight", "small_car_lheadlight.mesh", "small_car_headlight_uv", mLHeadlightNode);
    PhysicsCore::auto_scale_scenenode(mLHeadlightNode);

    createGeometry("CarEntity_RHeadlight", "small_car_rheadlight.mesh", "small_car_headlight_uv", mRHeadlightNode);
    PhysicsCore::auto_scale_scenenode(mRHeadlightNode);

    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "small_car_lwheel.mesh", "small_car_wheel_uv", mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "small_car_rwheel.mesh", "small_car_wheel_uv", mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "small_car_lwheel.mesh", "small_car_wheel_uv", mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "small_car_rwheel.mesh", "small_car_wheel_uv", mRRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void SmallCar::initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = mSceneMgr->createEntity("SmallCarCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "small_car_collision.mesh");
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

    //mCarChassis->showDebugShape( false );
}


/// @brief  Attaches 4 wheels to the car chassis.
void SmallCar::initWheels()
{
    float wheelBaseLength = 1.0245f;
    float wheelBaseHalfWidth  = 0.5625f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = -0.17;

    Ogre::Vector3 wheelDirectionCS0(0,-1,0);
    Ogre::Vector3 wheelAxleCS(-1,0,0);

    bool isFrontWheel = true;
    
    // Wheel 0 - Front Left
    Ogre::Vector3 connectionPointCS0 (wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength);
    mVehicle->addWheel(mFLWheelNode, connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius,
        isFrontWheel, mWheelFriction, mRollInfluence);

    // Wheel 1 - Front Right
    connectionPointCS0 = Ogre::Vector3(-wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength);
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

