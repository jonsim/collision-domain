/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

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

#define CRITICAL_DAMPING_COEF       0.3f

#define TRUCK_VTX_COUNT 20

static btScalar TruckVtx[] = {
    -124.954f, -56.6976f, 76.8711f,
    -124.954f, 24.5182f, 79.2062f,
    130.475f, 24.5182f, 79.2062f,
    130.475f, -56.6976f, 76.8711f,
    -124.954f, -25.7872f, -478.201f,
    -124.954f, 24.5182f, -478.201f,
    130.475f, 24.5182f, -478.201f,
    130.475f, -25.7872f, -478.201f,
    130.475f, -42.6501f, -133.55f,
    130.475f, 24.5182f, -133.55f,
    -124.954f, 24.5182f, -133.55f,
    -124.954f, -42.6501f, -133.55f,
    114.383f, 197.797f, -133.55f,
    -108.862f, 197.797f, -133.55f,
    -108.862f, 188.796f, 44.8438f,
    114.383f, 188.796f, 44.8438f,
    121.33f, 86.4506f, 79.3882f,
    -115.809f, 86.4506f, 79.3882f,
    -115.809f, 86.4506f, -133.55f,
    121.33f, 86.4506f, -133.55f,
};

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void TruckCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    mBrakingForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =   50.0f;
    mSuspensionDamping      =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness);
    mSuspensionCompression  =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness) + 0.2;
    mMaxSuspensionForce     =   70000.0f;
    mRollInfluence          =   0.1f;
    mSuspensionRestLength   =   0.25f;
    mMaxSuspensionTravelCm  =   7.5f;
    mFrictionSlip           =   3.0f;
	mChassisLinearDamping   =   0.2f;
	mChassisAngularDamping  =   0.2f;
	mChassisRestitution		=   0.6f;
	mChassisFriction        =   0.6f;
	mChassisMass            =   7000.0f;

    mWheelRadius      =  0.523f; // this is actually diameter!!
    mWheelWidth       =  0.243f;
    mWheelFriction    =  2.0f;//1000;//1e30f;
    mConnectionHeight =  0.6f; // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement = 0.015f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 15000.0f;
    mMaxBrakeForce = 300.0f;

	mFrontWheelDrive = true;
	mRearWheelDrive  = true;
    
    mGearCount = 9;
    mCurrentGear = 1;
    mGearRatio[0] = 14.00f;
    mGearRatio[1] = 12.00f;
    mGearRatio[2] = 10.00f;
    mGearRatio[3] = 07.00f;
    mGearRatio[4] = 05.60f;
    mGearRatio[5] = 04.20f;
    mGearRatio[6] = 02.25f;
    mGearRatio[7] = 01.20f;
    mGearRatio[8] = 00.80f;
    mReverseRatio = 03.00f;
    mFinalDriveRatio = 1.5f;

    mRevTick  = 500;
    mRevLimit = 3500;

    readTuning( "spec_truck.txt" );
}

/// @brief  Constructor to create a car, add its graphical model to ogre and add its physics model to bullet.
/// @param  sceneMgr     The Ogre graphics world.
/// @param  world        The bullet physics world.
/// @param  uniqueCarID  A unique ID for the car so that generated nodes do not have (forbidden) name collisions.
TruckCar::TruckCar(int uniqueCarID, CarSkin skin, bool silentCar)
{
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    btTransform chassisShift( btQuaternion::getIdentity(), btVector3( 0, 0.81f, 1.75f ) );

    initTuning();
    initNodes();
    initGraphics();
    initBody(carPosition, chassisShift);
    initWheels();
    initDoors(chassisShift);

    testCar = NULL; /*new SmallCar( sceneMgr, world, GameCore::mPhysicsCore->getUniqueEntityID() );

    btPoint2PointConstraint *constr = new btPoint2PointConstraint( 
        *getVehicle()->getBulletVehicle()->getRigidBody(), 
        *testCar->getVehicle()->getBulletVehicle()->getRigidBody(), 
        btVector3(0,0.2,-2.9), btVector3(0,0.2,1.7) );

    GameCore::mPhysicsCore->mWorld->getBulletDynamicsWorld()->addConstraint( constr, true );*/

    fricConst = new WheelFrictionConstraint( mVehicle, mCarChassis );
    fricConst->enableFeedback( true );

    GameCore::mPhysicsCore->getWorld()->addConstraint( fricConst );

    mHornSound = GameCore::mAudioCore->getSoundInstance(HORN_LOW, mUniqueCarID, NULL);

    // pitch is in play rate increase (4x max) (100 = 3.976x play rate)
    mEngineSound = GameCore::mAudioCore->getSoundInstance(ENGINE_TRUCK, mUniqueCarID, NULL, true);
    mEngineSound->setPitch(2.0f);
    
    #ifdef COLLISION_DOMAIN_CLIENT
        if (!silentCar) mEngineSound->play();
    #endif

    //mBodyNode->attachObject(mEngineSound);
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
TruckCar::~TruckCar(void)
{
    // Cleanup Bodies:
    delete mVehicle;
    delete mVehicleRayCaster;
    delete mCarChassis;

    // Cleanup Shapes:
    delete compoundChassisShape;
    delete chassisShape;

    GameCore::mAudioCore->deleteSoundInstance(mHornSound);
}


void TruckCar::louderLocalSounds() {
    float increaseTo = mEngineSound->getVolume() + 0.3;
    if (increaseTo < 1) mEngineSound->setVolume(increaseTo);
}


void TruckCar::frameEvent()
{
    float maxPitch = 2.0f;

    // for the multiplication later
    maxPitch -= 1;

    updateRPM();
    float rpm = this->getRPM();
    // min 300    max 3666

    rpm -= 300;
    if (rpm < 0) rpm = 0;
    float pitch = (rpm / 3366.f);
    if (pitch > 1) pitch = 1;
    // pitch between 0 and 1. add 1 so min. pitch is 1.0 as required to avoid phasey sounds
    pitch = pitch * maxPitch + 1;

    mEngineSound->setPitch(pitch);
    
    mEngineSound->setPosition(mBodyNode->getPosition());
    mEngineSound->setVelocity(Car::getLinearVelocity());
}


void TruckCar::playCarHorn()
{
    GameCore::mAudioCore->playSoundOrRestart(mHornSound);
}


/// @brief  Initialises the node tree for this car.
void TruckCar::initNodes()
{
    mPlayerNode  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode   = mPlayerNode->createChildSceneNode("BodyNode"   + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode     = mBodyNode->createChildSceneNode("ChassisNode"     + boost::lexical_cast<std::string>(mUniqueCarID));
    //mLDoorNode       = mBodyNode->createChildSceneNode("LDoorNode"       + boost::lexical_cast<std::string>(mUniqueCarID));
    mLDoorNode       = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("LDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    //mRDoorNode       = mBodyNode->createChildSceneNode("RDoorNode"       + boost::lexical_cast<std::string>(mUniqueCarID));
    mRDoorNode       = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("RDoorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode     = mBodyNode->createChildSceneNode("RBumperNode"     + boost::lexical_cast<std::string>(mUniqueCarID));
    mLWingmirrorNode = mBodyNode->createChildSceneNode("LWingmirrorNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRWingmirrorNode = mBodyNode->createChildSceneNode("RWingmirrorNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
	
	// Setup particles.
    mExhaustSystem = GameCore::mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Banger/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);
	mDustSystem    = GameCore::mSceneMgr->createParticleSystem("Dust"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Dust");
	mBodyNode->attachObject(mDustSystem);
    // The dust emitters should be placed in the location of the wheel nodes but since
    // the wheels nodes are not currently positioned correctly these are hard coded numbers.
    mDustSystem->getEmitter(0)->setPosition(Ogre::Vector3( 1.2f, 0.2f,  1.7f));  // FL
    mDustSystem->getEmitter(1)->setPosition(Ogre::Vector3(-1.2f, 0.2f,  1.7f));  // FR
    mDustSystem->getEmitter(2)->setPosition(Ogre::Vector3( 1.2f, 0.2f, -1.7f));  // RL
    mDustSystem->getEmitter(3)->setPosition(Ogre::Vector3(-1.2f, 0.2f, -1.7f));  // RR

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void TruckCar::initGraphics()
{
    // Load the truck meshes.
    createGeometry("UnIqUe_TruckBody",      "truck_body.mesh",        mChassisNode,     true);
    createGeometry("UnIqUe_TruckLDoor",     "truck_ldoor.mesh",       mLDoorNode,       true);
    createGeometry("UnIqUe_TruckRDoor",     "truck_rdoor.mesh",       mRDoorNode,       true);
    createGeometry("UnIqUe_TruckRBumper",   "truck_rbumper.mesh",     mRBumperNode,     true);
    createGeometry("CarEntity_LWingmirror", "truck_lwingmirror.mesh", mLWingmirrorNode, false);
    createGeometry("CarEntity_RWingmirror", "truck_rwingmirror.mesh", mRWingmirrorNode, false);
    createGeometry("CarEntity_FLWheel",     "truck_lwheel.mesh",      mFLWheelNode,     false);
    createGeometry("CarEntity_FRWheel",     "truck_rwheel.mesh",      mFRWheelNode,     false);
    createGeometry("CarEntity_RLWheel",     "truck_lwheel.mesh",      mRLWheelNode,     false);
    createGeometry("CarEntity_RRWheel",     "truck_rwheel.mesh",      mRRWheelNode,     false);

    // Scale the loaded meshes
    PhysicsCore::auto_scale_scenenode(mChassisNode);
    PhysicsCore::auto_scale_scenenode(mLDoorNode);
    PhysicsCore::auto_scale_scenenode(mRDoorNode);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);
    PhysicsCore::auto_scale_scenenode(mLWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mRWingmirrorNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
}


void TruckCar::updateTeam (int teamNumber)
{
    // Load the team coloured items
    switch (teamNumber)
    {
    case 1:
        setMaterial("truck_body_t1",  mChassisNode);
        setMaterial("truck_door_t1", mLDoorNode);
        setMaterial("truck_door_t1", mRDoorNode);
        break;
    case 2:
        setMaterial("truck_body_t2",  mChassisNode);
        setMaterial("truck_door_t2", mLDoorNode);
        setMaterial("truck_door_t2", mRDoorNode);
        break;
    default:
        break;
    }
}


void TruckCar::loadDestroyedModel (void)
{
    mChassisNode->detachAllObjects();
    createGeometry("CarEntity_Burnt", "truck_burnt.mesh", mChassisNode, false);
    makeBitsFallOff();
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void TruckCar::initBody(Ogre::Vector3 carPosition, btTransform& chassisShift)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("TruckCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "truck_collision.mesh");
    entity->setDebugDisplayEnabled( false );
    compoundChassisShape = new btCompoundShape();

    btConvexHullShape *convexHull = new btConvexHullShape( TruckVtx, TRUCK_VTX_COUNT, 3*sizeof(btScalar) );
    convexHull->setLocalScaling( btVector3( MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT ) );

    // Shift the mesh (this does work in a physical sense, but the wireframe is still drawn in the wrong place)
    compoundChassisShape->addChildShape( chassisShift, convexHull );

    btVector3 inertia;
    compoundChassisShape->calculateLocalInertia( mChassisMass, inertia );

    //BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState( mBodyNode );
    mState = new CarState( mBodyNode );

    mCarChassis = new btRigidBody( mChassisMass, mState, compoundChassisShape, inertia );
    GameCore::mPhysicsCore->addRigidBody( mCarChassis, COL_CAR, COL_CAR | COL_ARENA | COL_POWERUP );

    mCarChassis->setDamping(mChassisLinearDamping, mChassisAngularDamping);
    mCarChassis->setActivationState( DISABLE_DEACTIVATION );

    mTuning.m_frictionSlip             = mWheelFriction;
    mTuning.m_maxSuspensionForce       = mMaxSuspensionForce;
    mTuning.m_maxSuspensionTravelCm    = mMaxSuspensionTravelCm;
    mTuning.m_suspensionCompression    = mSuspensionCompression;
    mTuning.m_suspensionDamping        = mSuspensionDamping;
    mTuning.m_suspensionStiffness      = mSuspensionStiffness;
    
    mVehicleRayCaster = new btDefaultVehicleRaycaster( GameCore::mPhysicsCore->getWorld() );
    mVehicle = new Vehicle( mTuning, mCarChassis, mVehicleRayCaster );
    mState->setVehicle( mVehicle );

    // This line is needed otherwise the model appears wrongly rotated.
    mVehicle->setCoordinateSystem(0, 1, 2); // rightIndex, upIndex, forwardIndex

    GameCore::mPhysicsCore->getWorld()->addVehicle( mVehicle );
}

void TruckCar::initDoors( btTransform& chassisShift )
{
    btBoxShape *doorShape = new btBoxShape( btVector3( 0.005f, 0.82f, 0.55f ) );

    btCompoundShape *leftDoor  = new btCompoundShape();
    btCompoundShape *rightDoor = new btCompoundShape();

    btTransform ltrans( btQuaternion::getIdentity(), btVector3(  1.118f, 1.714f , 1.75f ) );
    btTransform rtrans( btQuaternion::getIdentity(), btVector3( -1.062f, 1.714f , 1.75f ) );

    leftDoor ->addChildShape( ltrans, doorShape );
    rightDoor->addChildShape( rtrans, doorShape );

    btMotionState *lstate = new BtOgre::RigidBodyState( mLDoorNode );
    btMotionState *rstate = new BtOgre::RigidBodyState( mRDoorNode );

    btVector3 linertia, rinertia;
    leftDoor ->calculateLocalInertia( 5.0f, linertia );
    rightDoor->calculateLocalInertia( 5.0f, rinertia );

    mLeftDoorBody  = new btRigidBody( 5.0f, lstate, leftDoor,  linertia );
    mRightDoorBody = new btRigidBody( 5.0f, rstate, rightDoor, rinertia );

    GameCore::mPhysicsCore->addRigidBody( mLeftDoorBody,  COL_CAR, COL_ARENA | COL_CAR );
    GameCore::mPhysicsCore->addRigidBody( mRightDoorBody, COL_CAR, COL_ARENA | COL_CAR );

    mLeftDoorBody->setDamping( 0.2f, 0.5f );
    mLeftDoorBody->setActivationState( DISABLE_DEACTIVATION );

    mRightDoorBody->setDamping( 0.2f, 0.5f );
    mRightDoorBody->setActivationState( DISABLE_DEACTIVATION );

    btContactSolverInfo& solverInfo = 
        GameCore::mPhysicsCore->getWorld()->getSolverInfo();

    solverInfo.m_numIterations = 160;

    leftDoorHinge = new btHingeConstraint( 
        *mCarChassis,
        *mLeftDoorBody,
        btVector3( 1.118f, 1.714f, 2.315f ),
        btVector3( 1.118f, 1.714f, 2.315f ),
        btVector3( 0.000f, 1.000f, 0.000f ),
        btVector3( 0.000f, 1.000f, 0.000f ) );


    rightDoorHinge = new btHingeConstraint( 
        *mCarChassis,
        *mRightDoorBody,
        btVector3( -1.062f, 1.714f, 2.315f ),
        btVector3( -1.062f, 1.714f, 2.315f ),
        btVector3( 0.0f, 1.0f, 0.0f ),
        btVector3( 0.0f, 1.0f, 0.0f ) );

    //leftDoorHinge->setParam( BT_CONSTRAINT_STOP_CFM, 0.6f );
    leftDoorHinge->setParam( BT_CONSTRAINT_STOP_ERP, 0.6f );

    leftDoorHinge->setLimit( 0.0f, (Ogre::Math::PI * 0.25f), 0.9f, 0.3f, 1.0f );
    rightDoorHinge->setLimit( -(Ogre::Math::PI * 0.25f), 0.0f, 0.9f, 0.01f, 0.0f );

    leftDoorHinge->enableFeedback( true );
    rightDoorHinge->enableFeedback( true );

    GameCore::mPhysicsCore->getWorld()->addConstraint( leftDoorHinge, true );

    GameCore::mPhysicsCore->getWorld()->addConstraint( rightDoorHinge, true );
}


/// @brief  Attaches 4 wheels to the car chassis.
void TruckCar::initWheels()
{
    float wheelBaseLength = 1.788f;
    float wheelBaseHalfWidth  = 1.05f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = -0.575f;

    btVector3 wheelDirectionCS0(0,-1,0);
    btVector3 wheelAxleCS(-1,0,0);

    bool isFrontWheel = true;
    
    // Wheel 0 - Front Left
    btVector3 connectionPointCS0 (wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength);
    mVehicle->addWheel( connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius, mTuning, isFrontWheel );
    mState->setWheel( 0, mFLWheelNode, BtOgre::Convert::toOgre( connectionPointCS0 ) );

    // Wheel 1 - Front Right
    connectionPointCS0 = btVector3(-wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ + wheelBaseLength);
    mVehicle->addWheel( connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius, mTuning, isFrontWheel );
    mState->setWheel( 1, mFRWheelNode, BtOgre::Convert::toOgre( connectionPointCS0 ) );
                    
    isFrontWheel = false;

    // Wheel 2 - Rear Right
    connectionPointCS0 = btVector3(-wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ - wheelBaseLength);
    mVehicle->addWheel( connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius, mTuning, isFrontWheel );
    mState->setWheel( 2, mRRWheelNode, BtOgre::Convert::toOgre( connectionPointCS0 ) );

    // Wheel 3 - Rear Left
    connectionPointCS0 = btVector3(wheelBaseHalfWidth, mConnectionHeight, wheelBaseShiftZ - wheelBaseLength);
    mVehicle->addWheel( connectionPointCS0, wheelDirectionCS0, wheelAxleCS, mSuspensionRestLength, mWheelRadius, mTuning, isFrontWheel );
    mState->setWheel( 3, mRLWheelNode, BtOgre::Convert::toOgre( connectionPointCS0 ) );

    for( int i=0; i < mVehicle->getNumWheels(); i++ )
	{
		btWheelInfo& wheel                  = mVehicle->getWheelInfo( i );
		wheel.m_suspensionStiffness         = mSuspensionStiffness;
		wheel.m_wheelsDampingRelaxation     = mSuspensionDamping;
		wheel.m_wheelsDampingCompression    = mSuspensionCompression;
        wheel.m_maxSuspensionForce          = mMaxSuspensionForce;
		wheel.m_frictionSlip                = mWheelFriction;
		wheel.m_rollInfluence               = mRollInfluence;
	}
}

void TruckCar::makeBitsFallOff()
{
    //mBodyNode->removeChild( "FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID) );
    removePiece( mLDoorNode, mLDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3(  1.094f, 1.665f, 1.768f ) );
    removePiece( mRDoorNode, mRDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3( -1.094f, 1.665f, 1.768f ) );

    removePiece( mRBumperNode, mRBumperBody, btVector3( 0.773f, 0.25f, 0.25f ), btVector3( 0.0f, 0.547f, 3.035f ) );
}