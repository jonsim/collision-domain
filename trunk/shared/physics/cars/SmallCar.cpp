/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"


/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void SmallCar::initTuning()
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
	mChassisLinearDamping   =   0.2f;
	mChassisAngularDamping  =   0.2f;
	mChassisRestitution		=   0.6f;
	mChassisFriction        =   0.6f;
	mChassisMass            = 800.0f;

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
SmallCar::SmallCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    Ogre::Vector3 chassisShift(0, 1.1f, 0.0f);

    initTuning();
    initNodes();
    initGraphics(chassisShift);
    initBody(carPosition, chassisShift);
    initWheels();
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
}


/// @brief  Attaches 4 wheels to the car chassis.
void SmallCar::initWheels()
{
    Ogre::Vector3 wheelDirectionCS0(0,-1,0);
    Ogre::Vector3 wheelAxleCS(-1,0,0);

    #define CUBE_HALF_EXTENTS 1.18
    bool isFrontWheel = true;
    
    // Wheel 1 - Front Left
    Ogre::Vector3 connectionPointCS0 (CUBE_HALF_EXTENTS-(0.3*mWheelWidth), mConnectionHeight, (2*CUBE_HALF_EXTENTS-mWheelRadius) +0.2);
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

