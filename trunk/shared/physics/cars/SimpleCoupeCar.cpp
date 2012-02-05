/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;
using namespace Ogre;

/*  Width:    1886mm (car not modelled with wing mirrors)
    Height:    1514mm (inc wheels, body alone is 1286mm)
    Length:    5110mm
    Wheelbase:    2939mm
    Weight:    1451kg
    Engine:    5.9 litre, 315bhp
    Tyre Diameter: 690mm
    Tyre Width: 176mm (bit that touches ground, not bounding box)
*/

#define CRITICAL_DAMPING_COEF       0.5f

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void SimpleCoupeCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    mBrakingForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =   60.0f;
    mSuspensionDamping      =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness);
    mSuspensionCompression  =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness) + 0.2;
    mMaxSuspensionForce     =   14000.0f;
    mRollInfluence          =    0.35f;
    mSuspensionRestLength   =    0.3f;
    mMaxSuspensionTravelCm  =   15.0f;
    mFrictionSlip           =   10.5f;
	mChassisLinearDamping   =    0.2f;
	mChassisAngularDamping  =    0.2f;
	mChassisRestitution		=    0.6f;
	mChassisFriction        =    0.6f;
	mChassisMass            = 1451.0f;

    mWheelRadius      =  0.345f; // this is actually diameter!!
    mWheelWidth       =  0.176f;
    mWheelFriction    =  1.8f;//1000;//1e30f;
    mConnectionHeight =  0.6f; // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement = 0.015f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 8000.0f;
    mMaxBrakeForce = 100.0f;
	
	mFrontWheelDrive = false;
	mRearWheelDrive  = true;

    mGearCount = 5;
    mCurrentGear = 1;
    mGearRatio[0] = 3.20f;
    mGearRatio[1] = 2.19f;
    mGearRatio[2] = 1.41f;
    mGearRatio[3] = 1.00f;
    mGearRatio[4] = 0.83f;
    mGearRatio[5] = 0.0f;
    mGearRatio[6] = 0.0f;
    mGearRatio[7] = 0.0f;
    mGearRatio[8] = 0.0f;
    mReverseRatio = 3.16f;
    mFinalDriveRatio = 3.06f;

    // http://www.allpar.com/cars/dodge/challenger/specifications.html

    mRevTick  = 1800;
    mRevLimit = 6000;

    readTuning( "spec_banger.txt" );
}


/// @brief  Constructor to create a car, add its graphical model to ogre and add its physics model to bullet.
/// @param  sceneMgr     The Ogre graphics world.
/// @param  world        The bullet physics world.
/// @param  uniqueCarID  A unique ID for the car so that generated nodes do not have (forbidden) name collisions.
SimpleCoupeCar::SimpleCoupeCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    Ogre::Vector3 chassisShift(0, 0.5f, 0.5f);

    initTuning();
    initNodes();
    initGraphics(chassisShift);
    initBody(carPosition, chassisShift);
    initWheels();

    WheelFrictionConstraint *fricConst = new WheelFrictionConstraint( mVehicle, mbtRigidBody );
    GameCore::mPhysicsCore->mWorld->getBulletDynamicsWorld()->addConstraint( fricConst );
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
SimpleCoupeCar::~SimpleCoupeCar(void)
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


void SimpleCoupeCar::playCarHorn()
{
    GameCore::mAudioCore->playCarHorn(HORN_MID);
}


/// @brief  Initialises the node tree for this car.
void SimpleCoupeCar::initNodes()
{
    mPlayerNode  = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFLDoorNode   = mBodyNode->createChildSceneNode("FLDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRDoorNode   = mBodyNode->createChildSceneNode("FRDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLDoorNode   = mBodyNode->createChildSceneNode("RLDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRDoorNode   = mBodyNode->createChildSceneNode("RRDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
	
	// setup particles. This needs to be propogated.
    mExhaustSystem = mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "Banger/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void SimpleCoupeCar::initGraphics(Ogre::Vector3 chassisShift)
{
    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    createGeometry("CarBody", "banger_body.mesh", "banger_body_uv", mChassisNode);
    PhysicsCore::auto_scale_scenenode(mChassisNode);

    // load the left door baby
    createGeometry("CarEntity_LDoor", "banger_fldoor.mesh", "banger_fdoor_uv", mFLDoorNode);
    PhysicsCore::auto_scale_scenenode(mFLDoorNode);
    
    // lets get a tasty right door
    createGeometry("CarEntity_RDoor", "banger_frdoor.mesh", "banger_fdoor_uv", mFRDoorNode);
    PhysicsCore::auto_scale_scenenode(mFRDoorNode);

    // load the left door baby
    createGeometry("CarEntity_RLDoor", "banger_rldoor.mesh", "banger_rdoor_uv", mRLDoorNode);
    PhysicsCore::auto_scale_scenenode(mRLDoorNode);
    
    // lets get a tasty right door
    createGeometry("CarEntity_RRDoor", "banger_rrdoor.mesh", "banger_rdoor_uv", mRRDoorNode);
    PhysicsCore::auto_scale_scenenode(mRRDoorNode);

    // and now a sweet sweet front bumper
    createGeometry("CarEntity_FBumper", "banger_fbumper.mesh", "banger_bumper", mFBumperNode);
    PhysicsCore::auto_scale_scenenode(mFBumperNode);

    // and now a regular rear bumper
    createGeometry("CarEntity_RBumper", "banger_rbumper.mesh", "banger_bumper", mRBumperNode);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);

    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "banger_lwheel.mesh", "banger_wheel_uv", mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "banger_rwheel.mesh", "banger_wheel_uv", mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "banger_lwheel.mesh", "banger_wheel_uv", mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "banger_rwheel.mesh", "banger_wheel_uv", mRRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void SimpleCoupeCar::initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = mSceneMgr->createEntity("SmallCarCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "banger_collision.mesh");
    entity->setDebugDisplayEnabled( false );
    compoundChassisShape = new OgreBulletCollisions::CompoundCollisionShape();

    // Transformation matrix to scale the imported mesh
    Ogre::Matrix4 matScale(MESH_SCALING_CONSTANT, 0, 0, 0, 0, MESH_SCALING_CONSTANT, 0, 0, 0, 0, MESH_SCALING_CONSTANT, 0, 0, 0, 0, 1.0);

    // Create a compound shape from the mesh's vertices
    OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = 
        new OgreBulletCollisions::StaticMeshToShapeConverter(entity, matScale);

    OgreBulletCollisions::CompoundCollisionShape *tmp = trimeshConverter->createConvexDecomposition( 5U, 5.0F, 15.0F, 20U, 0.0F );

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

    mTuning->getBulletTuning()->m_maxSuspensionForce = mMaxSuspensionForce;

    mVehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(mWorld);
    
    mVehicle = new OgreBulletDynamics::RaycastVehicle(mCarChassis, mTuning, mVehicleRayCaster);
    
    // This line is needed otherwise the model appears wrongly rotated.
    mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex
    
    mbtRigidBody = mCarChassis->getBulletRigidBody();

    OgreBulletCollisions::DebugCollisionShape *dbg = mCarChassis->getDebugShape();

    Ogre::Matrix4 matChassisShift;
    matChassisShift.makeTrans( chassisShift );
    dbg->setWorldTransform( matChassisShift );

    mCarChassis->showDebugShape( false );
}


/// @brief  Attaches 4 wheels to the car chassis.
void SimpleCoupeCar::initWheels()
{
    float wheelBaseLength = 1.4695f;
    float wheelBaseHalfWidth  = 0.85f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = 0.10f;

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

