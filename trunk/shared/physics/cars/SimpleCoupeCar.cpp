/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "SharedIncludes.h"

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

#define BANGER_VTX_COUNT 29

static btScalar BangerVtx[] = {
    85.8457f, -6.20191f, 182.692f,
    85.8457f, -5.46665f, -305.179f,
    83.076f, 45.5736f, 183.587f,
    83.076f, 47.7641f, -306.054f,
    -83.076f, 45.5736f, 183.587f,
    -83.076f, 47.7641f, -306.054f,
    -85.8457f, -6.20191f, 182.692f,
    -85.8457f, -5.46665f, -305.179f,
    83.076f, 52.7043f, 27.6737f,
    -83.076f, 52.7043f, 27.6737f,
    81.3123f, 56.7436f, -216.481f,
    -81.3123f, 56.7436f, -216.481f,
    60.2266f, 101.369f, -165.804f,
    -60.2266f, 101.369f, -165.804f,
    -60.2266f, 100.685f, -40.2496f,
    -60.2266f, 100.685f, -40.2496f,
    60.2266f, 100.685f, -40.2496f,
    85.8457f, -23.8549f, 61.4045f,
    -85.8457f, -23.8549f, 61.4045f,
    85.8457f, -20.1567f, -207.338f,
    -85.8457f, -20.1567f, -207.338f,
    92.9599f, 31.761f, 183.139f,
    94.6668f, 26.4999f, 44.5391f,
    94.1102f, 30.3687f, -211.909f,
    94.6668f, 33.2239f, -305.617f,
    -94.6668f, 26.4999f, 44.5391f,
    -94.1102f, 30.3687f, -211.909f,
    -92.9599f, 31.761f, 183.139f,
    -94.6668f, 33.2239f, -305.617f,
};

/// @brief  Tuning values to create a car which handles well and matches the "type" of car we're trying to create.
void SimpleCoupeCar::initTuning()
{
    // mTuning related values
    mSteer = 0.0f;
    mEngineForce = 0.0f;
    mBrakingForce = 0.0f;
    
    // mTuning fixed properties
    mSuspensionStiffness    =   30.0f;
    mSuspensionDamping      =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness);
    mSuspensionCompression  =   CRITICAL_DAMPING_COEF * 2 * btSqrt(mSuspensionStiffness) + 0.2;
    mMaxSuspensionForce     = 8000.00f;
    mRollInfluence          =    0.35f;
    mSuspensionRestLength   =    0.3f;
    mMaxSuspensionTravelCm  =   15.0f;
    mFrictionSlip           =    2.0f;
	mChassisLinearDamping   =    0.2f;
	mChassisAngularDamping  =    0.2f;
	mChassisRestitution		=    0.6f;
	mChassisFriction        =    0.6f;
	mChassisMass            = 1451.0f;

    mWheelRadius      =  0.345f; // this is actually diameter!!
    mWheelWidth       =  0.176f;
    mWheelFriction    =  3.0f;//1000;//1e30f;
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

	mMaxSpeed = 80.0;

    readTuning( "spec_banger.txt" );
}


/// @brief  Constructor to create a car, add its graphical model to ogre and add its physics model to bullet.
/// @param  sceneMgr     The Ogre graphics world.
/// @param  world        The bullet physics world.
/// @param  uniqueCarID  A unique ID for the car so that generated nodes do not have (forbidden) name collisions.
SimpleCoupeCar::SimpleCoupeCar(int uniqueCarID, CarSkin skin, bool silentCar)
{
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    btTransform chassisShift( btQuaternion::getIdentity(), btVector3( 0, 0.5f, 0.5f) );

    initTuning();
    initNodes();
    initGraphics(chassisShift, skin);
    initBody(carPosition, chassisShift);
    initWheels();

    mLeftDoorBody = NULL;
    mRightDoorBody = NULL;
    leftDoorHinge = NULL;
    testCar = NULL;

    fricConst = new WheelFrictionConstraint( mVehicle, mCarChassis );
    fricConst->enableFeedback( true );

    GameCore::mPhysicsCore->getWorld()->addConstraint( fricConst );

    mHornSound = GameCore::mAudioCore->getSoundInstance(HORN_MID, mUniqueCarID, NULL);
    //mHornSound->setPosition(Ogre::Vector3(0,0,0));
    //mHornSound->setRolloffFactor(2.f);
    //mHornSound->setReferenceDistance(10.f);

    // pitch is in play rate increase (4x max) (100 = 3.976x play rate)
    mEngineSound = GameCore::mAudioCore->getSoundInstance(ENGINE_COUPE, mUniqueCarID, NULL, true);
    mEngineSound->setPitch(2.0f);
    
    #ifdef COLLISION_DOMAIN_CLIENT
        if (!silentCar) mEngineSound->play();
    #endif

    //mBodyNode->attachObject(mEngineSound);
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
SimpleCoupeCar::~SimpleCoupeCar(void)
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

/*static float min = 99999;
static float max = 0;
min = min > rpm ? rpm : min;
max = max < rpm ? rpm : max;
std::string s = "min ";
s += boost::lexical_cast<std::string>(min) + "    max " + boost::lexical_cast<std::string>(max) + "\n";
OutputDebugString(s.c_str());*/


void SimpleCoupeCar::louderLocalSounds() {
    //mEngineSound->setVolume(0); return;

    float increaseTo = mEngineSound->getVolume() + 0.25;
    if (increaseTo < 1) mEngineSound->setVolume(increaseTo);
}


void SimpleCoupeCar::frameEvent()
{
    float maxPitch = 1.0f;

    updateRPM();
    float rpm = this->getRPM();
    // min 1600    max 4497.07373

    rpm -= 1600;
    if (rpm < 0) rpm = 0;
    float pitch = (rpm / 2898.f);
    if (pitch > 1) pitch = 1;
    // pitch between 0 and 1. add 1 so min. pitch is 1.0 as required to avoid phasey sounds
    pitch = pitch * maxPitch + 1;

    mEngineSound->setPitch(pitch);

    mEngineSound->setPosition(mBodyNode->getPosition());
    mEngineSound->setVelocity(Car::getLinearVelocity());
}


void SimpleCoupeCar::playCarHorn()
{
    GameCore::mAudioCore->playSoundOrRestart(mHornSound);
}


/// @brief  Initialises the node tree for this car.
void SimpleCoupeCar::initNodes()
{
    mPlayerNode  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode"   + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFLDoorNode  = mBodyNode->createChildSceneNode("FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRDoorNode  = mBodyNode->createChildSceneNode("FRDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLDoorNode  = mBodyNode->createChildSceneNode("RLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRDoorNode  = mBodyNode->createChildSceneNode("RRDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
	
	// Setup particles.
    mExhaustSystem = GameCore::mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Banger/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);
    //Ogre::ParticleSystem* foo = GameCore::mSceneMgr->createParticleSystem("TestEffect" + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Sparks");
	//mBodyNode->attachObject(foo);
	mDustSystem    = GameCore::mSceneMgr->createParticleSystem("Dust"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Dust");
	mBodyNode->attachObject(mDustSystem);
    // The dust emitters should be placed in the location of the wheel nodes but since
    // the wheels nodes are not currently positioned correctly these are hard coded numbers.
    mDustSystem->getEmitter(0)->setPosition(Ogre::Vector3( 0.8f, 0.2f,  1.6f));  // FL
    mDustSystem->getEmitter(1)->setPosition(Ogre::Vector3(-0.8f, 0.2f,  1.6f));  // FR
    mDustSystem->getEmitter(2)->setPosition(Ogre::Vector3( 0.8f, 0.2f, -1.6f));  // RL
    mDustSystem->getEmitter(3)->setPosition(Ogre::Vector3(-0.8f, 0.2f, -1.6f));  // RR

    // The variables which aren't yet to be used
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void SimpleCoupeCar::initGraphics(btTransform& chassisShift, CarSkin skin)
{
    // Load the meshes.
	// true means it is deformable, therefore the unique entity name (defined in graphics code) needs to
	// be used in the first parameter
    createGeometry("UnIqUe_BangerBody",    "banger_body.mesh",    mChassisNode, true);
    createGeometry("UnIqUe_BangerLDoor",   "banger_fldoor.mesh",  mFLDoorNode,  true);
    createGeometry("UnIqUe_BangerRDoor",   "banger_frdoor.mesh",  mFRDoorNode,  true);
    createGeometry("UnIqUe_BangerRLDoor",  "banger_rldoor.mesh",  mRLDoorNode,  true);
    createGeometry("UnIqUe_BangerRRDoor",  "banger_rrdoor.mesh",  mRRDoorNode,  true);
    createGeometry("UnIqUe_BangerFBumper", "banger_fbumper.mesh", mFBumperNode, true);
    createGeometry("UnIqUe_BangerRBumper", "banger_rbumper.mesh", mRBumperNode, true);
    createGeometry("CarEntity_FLWheel", "banger_lwheel.mesh",     mFLWheelNode, false);
    createGeometry("CarEntity_FRWheel", "banger_rwheel.mesh",     mFRWheelNode, false);
    createGeometry("CarEntity_RLWheel", "banger_lwheel.mesh",     mRLWheelNode, false);
    createGeometry("CarEntity_RRWheel", "banger_rwheel.mesh",     mRRWheelNode, false);
    
    // Scale all loaded meshes.
    PhysicsCore::auto_scale_scenenode(mChassisNode);
    PhysicsCore::auto_scale_scenenode(mFLDoorNode);
    PhysicsCore::auto_scale_scenenode(mFRDoorNode);
    PhysicsCore::auto_scale_scenenode(mRLDoorNode);
    PhysicsCore::auto_scale_scenenode(mRRDoorNode);
    PhysicsCore::auto_scale_scenenode(mFBumperNode);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);

    if (skin == SKIN_TEAM1)
        updateTeam(1);
    else if (skin == SKIN_TEAM2)
        updateTeam(2);
}


void SimpleCoupeCar::updateTeam (int teamNumber)
{
    // Load the team coloured items
    switch (teamNumber)
    {
    case 1:
        setMaterial("banger_body_t1",  mChassisNode);
        setMaterial("banger_fdoor_t1", mFLDoorNode);
        setMaterial("banger_fdoor_t1", mFRDoorNode);
        setMaterial("banger_rdoor_t1", mRLDoorNode);
        setMaterial("banger_rdoor_t1", mRRDoorNode);
        break;
    case 2:
        setMaterial("banger_body_t2",  mChassisNode);
        setMaterial("banger_fdoor_t2", mFLDoorNode);
        setMaterial("banger_fdoor_t2", mFRDoorNode);
        setMaterial("banger_rdoor_t2", mRLDoorNode);
        setMaterial("banger_rdoor_t2", mRRDoorNode);
        break;
    default:
        break;
    }
}


void SimpleCoupeCar::loadDestroyedModel (void)
{
    mChassisNode->detachAllObjects();
    createGeometry("CarEntity_Burnt", "banger_burnt.mesh", mChassisNode, false);
    makeBitsFallOff();
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void SimpleCoupeCar::initBody(Ogre::Vector3 carPosition, btTransform& chassisShift)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("BangerCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "banger_collision.mesh");
    entity->setDebugDisplayEnabled( false );
    compoundChassisShape = new btCompoundShape();

    btConvexHullShape *convexHull = new btConvexHullShape( BangerVtx, BANGER_VTX_COUNT, 3*sizeof(btScalar) );
    convexHull->setLocalScaling( btVector3( MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT ) );

    // Shift the mesh (this does work in a physical sense, but the wireframe is still drawn in the wrong place)
    compoundChassisShape->addChildShape( chassisShift, convexHull );

    btVector3 inertia;
    compoundChassisShape->calculateLocalInertia( mChassisMass, inertia );

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


/// @brief  Attaches 4 wheels to the car chassis.
void SimpleCoupeCar::initWheels()
{
    float wheelBaseLength = 1.4695f;
    float wheelBaseHalfWidth  = 0.85f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = 0.10f;

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

void SimpleCoupeCar::makeBitsFallOff()
{
    //mBodyNode->removeChild( "FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID) );
    removePiece( mFLDoorNode, mFLDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3(  0.773f, 0.895f, 0.315f ) );
    removePiece( mFRDoorNode, mFRDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3( -0.773f, 0.895f, 0.315f ) );
    removePiece( mRLDoorNode, mRLDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3(  0.773f, 0.900f, 0.679f ) );
    removePiece( mRRDoorNode, mRRDoorBody, btVector3( 0.25f, 0.58f, 0.55f ), btVector3( -0.773f, 0.900f, 0.679f ) );

    removePiece( mFBumperNode, mFBumperBody, btVector3( 0.773f, 0.25f, 0.25f ), btVector3( 0.0f, 0.486f,  2.424f ) );
    removePiece( mRBumperNode, mRBumperBody, btVector3( 0.773f, 0.25f, 0.25f ), btVector3( 0.0f, 0.518f, -2.424f ) );
}
