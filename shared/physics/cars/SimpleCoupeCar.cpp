/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "Car.h"
#include "PhysicsCore.h"
#include "SimpleCoupeCar.h"
#include "Gameplay.h"
#include "GameCore.h"
#include "Vehicle.h"

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

#define BANGER_SPHERE_COUNT 17
static btScalar BangerSphere[] = {
    0,		0.682f,	2.216f,	0.4614f,
    0.632f,	0.682f,	2.175f,	0.4614f,
    -0.632f,0.682f,	2.175f,	0.4614f,
    0.594f,	0.654f,	1.488f,	0.5075f,
    0,		0.654f,	1.488f,	0.5075f,
    -0.594f,0.654f,	1.488f,	0.5075f,
    0.53f,	0.662f,	0.736f,	0.5611f,
    -0.53f,	0.662f,	0.736f,	0.5611f,
    0.285f,	0.901f,	0.089f,	0.7798f,
    -0.285f,0.901f,	0.089f,	0.7798f,
    0.286f,	0.919f,	-0.86f,	0.7798f,
    -0.286f,0.919f,	0.86f,	0.7798f,	
    0.536f,	0.686f,	-1.647f,0.5572f,
    -0.536f,0.686f,	-1.647f,0.5572f,	
    0.623f,	0.707f,	-2.23f,	0.4805f,
    0,		0.707f,	-2.237f,0.4805f,
    -0.623f,0.707f,	-2.23f,	0.4805f,
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
    mRollInfluence          =    1.0f;
    mSuspensionRestLength   =    0.3f;
    mMaxSuspensionTravelCm  =   15.0f;
    mFrictionSlip           =    2.0f;
	mChassisLinearDamping   =    0.2f;
	mChassisAngularDamping  =    0.0f;
	mChassisRestitution		=    0.2f;
	mChassisFriction        =    0.2f;
	mChassisMass            = 1451.0f;

    mWheelRadius      =  0.345f; // this is actually diameter!!
    mWheelWidth       =  0.176f;
    mWheelFriction    =  5.0f;//1000;//1e30f;
    mConnectionHeight =  0.6f; // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement = 0.015f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 8000.0f;
    mMaxBrakeForce = 100.0f;
    mMaxAccelForceBuf = mMaxAccelForce;
	
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
    mFinalDriveRatio = 3.6f; // 3.06 (it seems odd to do 40mph in 1st gear :P)

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
SimpleCoupeCar::SimpleCoupeCar(int uniqueCarID, TeamID tid, ArenaID aid) : Car(uniqueCarID),
                                                                           mHasLocalSounds(false)
{
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);

    initTuning();
    initNodes();
    #ifdef COLLISION_DOMAIN_CLIENT
        initGraphics(tid, aid);
    #endif
    initBody(carPosition);
    initWheels();

    mLeftDoorBody = NULL;
    mRightDoorBody = NULL;
    leftDoorHinge = NULL;
    testCar = NULL;

    fricConst = new WheelFrictionConstraint( mVehicle, mCarChassis );
    fricConst->enableFeedback( true );

    GameCore::mPhysicsCore->getWorld()->addConstraint( fricConst );
    
    #ifdef COLLISION_DOMAIN_CLIENT
        mHornSound = GameCore::mAudioCore->getSoundInstance(HORN_MID, mUniqueCarID, NULL);
    
        // pitch is in play rate increase (4x max) (100 = 3.976x play rate)
        mEngineSound = GameCore::mAudioCore->getSoundInstance(ENGINE_COUPE, mUniqueCarID, NULL, true);
        mEngineSound->setPitch(2.0f);
    #endif
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
SimpleCoupeCar::~SimpleCoupeCar(void)
{
    GameCore::mPhysicsCore->removeBody( mCarChassis );
    GameCore::mPhysicsCore->removeBody( mFLDoorBody );
    GameCore::mPhysicsCore->removeBody( mFRDoorBody );
    GameCore::mPhysicsCore->removeBody( mRLDoorBody );
    GameCore::mPhysicsCore->removeBody( mRRDoorBody );
    GameCore::mPhysicsCore->removeBody( mFBumperBody );
    GameCore::mPhysicsCore->removeBody( mRBumperBody );

    GameCore::mPhysicsCore->getWorld()->removeAction( mVehicle );
    
    // Destroy particle systems.
#ifdef COLLISION_DOMAIN_CLIENT
#ifdef PARTICLE_EFFECT_EXHAUST
    GameCore::mSceneMgr->destroyParticleSystem(mExhaustSystem);
#endif
#ifdef PARTICLE_EFFECT_DUST
    GameCore::mSceneMgr->destroyParticleSystem(mDustSystem);
#endif
#ifdef PARTICLE_EFFECT_SMOKE
    GameCore::mSceneMgr->destroyParticleSystem(mSmokeSystem);
#endif
#ifdef PARTICLE_EFFECT_FIRE
    GameCore::mSceneMgr->destroyParticleSystem(mFireSystem);
#endif
#endif

    mBodyNode->removeAndDestroyAllChildren();
    GameCore::mSceneMgr->destroySceneNode( mBodyNode );

    mWheelsNode->removeAndDestroyAllChildren();
    GameCore::mSceneMgr->destroySceneNode( mWheelsNode );

    GameCore::mPhysicsCore->getWorld()->removeConstraint( fricConst );

    // Cleanup Bodies:
    delete fricConst;
    delete mVehicle;
    delete mVehicleRayCaster;

    mVehicle = NULL;
    mVehicleRayCaster = NULL;

#ifdef COLLISION_DOMAIN_CLIENT
    mEngineSound->stop();

    GameCore::mAudioCore->deleteSoundInstance(mHornSound);
    GameCore::mAudioCore->deleteSoundInstance(mEngineSound);
#endif
}

void SimpleCoupeCar::createCollisionShapes()
{
    btTransform chassisShift( btQuaternion::getIdentity(), btVector3( 0, 0.5f, 0.5f) );
    btCompoundShape *compoundChassisShape = new btCompoundShape();
    //btConvexHullShape *convexHull = new btConvexHullShape( BangerVtx, BANGER_VTX_COUNT, 3*sizeof(btScalar) );
    //convexHull->setLocalScaling( btVector3( MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT ) );
    //compoundChassisShape->addChildShape( chassisShift, convexHull );

    for( int i = 0; i < BANGER_SPHERE_COUNT; i ++ )
    {
        btSphereShape *sphere = new btSphereShape( BangerSphere[i * 4 + 3] );
        btTransform sphereShift( btQuaternion::getIdentity(), btVector3( BangerSphere[i * 4], BangerSphere[i * 4 + 1], BangerSphere[i * 4 + 2] ) );
        compoundChassisShape->addChildShape( sphereShift, sphere );
    }

    //compoundChassisShape->addChildShape( btTransform( btQuaternion::getIdentity(), btVector3( 0, 0.6, 0 ) ), new btBoxShape( btVector3( 0.800, 0.25, 2.400 ) ) );
    btBoxShape *doorShape = new btBoxShape( btVector3( 0.25f, 0.58f, 0.55f ) );
    btBoxShape *fBumperShape = new btBoxShape( btVector3( 0.773f, 0.25f, 0.25f ) );
    btBoxShape *rBumperShape = new btBoxShape( btVector3( 0.773f, 0.25f, 0.25f ) );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_BANGER, compoundChassisShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_BANGER_DOOR, doorShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_BANGER_FBUMPER, fBumperShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_BANGER_RBUMPER, rBumperShape );
}

void SimpleCoupeCar::louderLocalSounds() {
    #ifdef COLLISION_DOMAIN_CLIENT
        if (mHasLocalSounds) return;

        mGearSound = GameCore::mAudioCore->getSoundInstance(GEAR_CHANGE, mUniqueCarID, NULL);

        mHasLocalSounds = true;

        float increaseTo = ENGINE_MAX_VOLUME + 0.25f;
        if (increaseTo >= 1)
            increaseTo = 1.0f;

        mEngineSound->setMaxVolume(increaseTo);
    
        mEngineSound->setRelativeToListener(true);
        mEngineSound->setPosition(0,0,0);
        mEngineSound->setVelocity(0,0,0);
    
        //Car::mGearSound->setRelativeToListener(true);
        Car::mGearSound->setPosition(0,0,0);
        Car::mGearSound->setVelocity(0,0,0);
    #endif
}


void SimpleCoupeCar::startEngineSound() {
    mEngineSound->setVolume(0);
    mEngineSound->startFade(true, 2.5f);
    
    // but fading it in will ensure that if it tries to play at some point in the first 2.5s, the vol will be low
    if (Car::mGearSound)
    {
        Car::mGearSound->setVolume(0);
        Car::mGearSound->startFade(true, 2.5f);
    }
    
}


// Only gets called on the client
void SimpleCoupeCar::updateAudioPitchFrameEvent()
{
#ifdef COLLISION_DOMAIN_CLIENT
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
    
    if (!mHasLocalSounds) {
        mEngineSound->setPosition(mBodyNode->getPosition());
        mEngineSound->setVelocity(Car::getLinearVelocity());

        //Car::mGearSound->setPosition(mBodyNode->getPosition());
        //Car::mGearSound->setVelocity(Car::getLinearVelocity());
    }
#endif
}


void SimpleCoupeCar::playCarHorn()
{
#ifdef COLLISION_DOMAIN_CLIENT
    Player *localPlayer = GameCore::mPlayerPool->getLocalPlayer();
    if (!localPlayer) return;

    Car *car = localPlayer->getCar();
    if (car && this == car)
    {
        GameCore::mAudioCore->playSoundOrRestart(mHornSound);
    }
#endif
}


/// @brief  Initialises the node tree for this car.
void SimpleCoupeCar::initNodes()
{
    // Player node.
    mPlayerNode  = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // Top level nodes.
    mBodyNode    = mPlayerNode->createChildSceneNode("BodyNode"   + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode  = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // Body nodes.
    mChassisNode = mBodyNode->createChildSceneNode("ChassisNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFLDoorNode  = mBodyNode->createChildSceneNode("FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRDoorNode  = mBodyNode->createChildSceneNode("FRDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLDoorNode  = mBodyNode->createChildSceneNode("RLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRDoorNode  = mBodyNode->createChildSceneNode("RRDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode = mBodyNode->createChildSceneNode("FBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode = mBodyNode->createChildSceneNode("RBumperNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // Wheel nodes.
    mFLWheelNode = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // Scale all nodes.
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
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void SimpleCoupeCar::initGraphics(TeamID tid, ArenaID aid)
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
    createGeometry("CarEntity_FLWheel",    "banger_lwheel.mesh",  mFLWheelNode, false);
    createGeometry("CarEntity_FRWheel",    "banger_rwheel.mesh",  mFRWheelNode, false);
    createGeometry("CarEntity_RLWheel",    "banger_lwheel.mesh",  mRLWheelNode, false);
    createGeometry("CarEntity_RRWheel",    "banger_rwheel.mesh",  mRRWheelNode, false);

	// Setup particles.
#ifdef PARTICLE_EFFECT_EXHAUST
    mExhaustSystem = GameCore::mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Banger/Exhaust");
	mBodyNode->attachObject(mExhaustSystem);
#endif
#ifdef PARTICLE_EFFECT_DUST
	mDustSystem    = GameCore::mSceneMgr->createParticleSystem("Dust"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Dust");
	mBodyNode->attachObject(mDustSystem);

    // Place the dust emitters. These should be placed in the location of the wheel nodes but since
    // the wheels nodes are not currently positioned correctly these are hard coded numbers.
    mDustSystem->getEmitter(0)->setPosition(Ogre::Vector3( 0.8f, 0.2f,  1.6f));  // FL
    mDustSystem->getEmitter(1)->setPosition(Ogre::Vector3(-0.8f, 0.2f,  1.6f));  // FR
    mDustSystem->getEmitter(2)->setPosition(Ogre::Vector3( 0.8f, 0.2f, -1.6f));  // RL
    mDustSystem->getEmitter(3)->setPosition(Ogre::Vector3(-0.8f, 0.2f, -1.6f));  // RR
#endif
#ifdef PARTICLE_EFFECT_SMOKE
    mSmokeSystem   = GameCore::mSceneMgr->createParticleSystem("Smoke"   + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Smoke");
	mBodyNode->attachObject(mSmokeSystem);

    // The smoke emitter should be placed over the engine.
    mSmokeSystem->getEmitter(0)->setPosition(Ogre::Vector3(0, 0, 0));
#endif
#ifdef PARTICLE_EFFECT_FIRE
    mFireSystem    = GameCore::mSceneMgr->createParticleSystem("Fire"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Fire");
	mBodyNode->attachObject(mFireSystem);

    // As should the fire emitter (which is, slightly more complex as it is a box).
    mFireSystem->getEmitter(0)->setParameter("width",  "2");
    mFireSystem->getEmitter(0)->setParameter("height", "2");    // height and depth are effectively switched
    mFireSystem->getEmitter(0)->setPosition(Ogre::Vector3(0.0f, 1.1f, 1.7f));
#endif
    
    // Update the skin based on the team
    updateTeam(tid);
    updateArena(aid);

    // The variables which aren't yet to be used <- what the hell are these?
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


void SimpleCoupeCar::updateTeam (TeamID tid)
{
    //Give each player no team if the game is FreeForAll
    if(GameCore::mGameplay->getGameMode() == FFA_MODE)
    {
        tid = NO_TEAM;
    }
    // Load the team coloured items
    switch (tid)
    {
    case NO_TEAM:
        setMaterial("banger_body_uv",  mChassisNode);
        setMaterial("banger_fdoor_uv", mFLDoorNode);
        setMaterial("banger_fdoor_uv", mFRDoorNode);
        setMaterial("banger_rdoor_uv", mRLDoorNode);
        setMaterial("banger_rdoor_uv", mRRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)    
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),0);
        #endif
        break;
    case BLUE_TEAM:
        setMaterial("banger_body_t1",  mChassisNode);
        setMaterial("banger_fdoor_t1", mFLDoorNode);
        setMaterial("banger_fdoor_t1", mFRDoorNode);
        setMaterial("banger_rdoor_t1", mRLDoorNode);
        setMaterial("banger_rdoor_t1", mRRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)    
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),1);
        #endif
        break;
    case RED_TEAM:
        setMaterial("banger_body_t2",  mChassisNode);
        setMaterial("banger_fdoor_t2", mFLDoorNode);
        setMaterial("banger_fdoor_t2", mFRDoorNode);
        setMaterial("banger_rdoor_t2", mRLDoorNode);
        setMaterial("banger_rdoor_t2", mRRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)            
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),2);
        #endif
        break;
    default:
        break;
    }
}


void SimpleCoupeCar::updateArena (ArenaID aid)
{
#ifdef PARTICLE_EFFECT_DUST
    Ogre::ColourValue dustColour;
    unsigned char i;

    // Load data depending on the current arena.
    if (aid == COLOSSEUM_ARENA)
        dustColour = Ogre::ColourValue(1.000f, 1.000f, 1.000f, 0.8f);
    else if (aid == FOREST_ARENA)
        dustColour = Ogre::ColourValue(0.663f, 0.525f, 0.439f, 1.0f);
    else // quarry
        dustColour = Ogre::ColourValue(0.500f, 0.500f, 0.680f, 1.0f);

    // Update the dust emitter.
    for (i = 0; i < 4; i++)
        mDustSystem->getEmitter(i)->setColour(dustColour);
#endif

    // Update the environment map.
#ifdef TEXTURE_ENV_SWAPPING
    std::string newSphereMap = (aid == COLOSSEUM_ARENA) ? "arena1_spheremap.jpg" : ((aid == FOREST_ARENA) ? "arena2_spheremap.jpg" : "arena3_spheremap.jpg");
    std::string materialList[10] = {"banger_body_uv", "banger_fdoor_uv", "banger_rdoor_uv",
                                    "banger_body_t1", "banger_fdoor_t1", "banger_rdoor_t1",
                                    "banger_body_t2", "banger_fdoor_t2", "banger_rdoor_t2",
                                    "banger_bumper"};
    for (unsigned char i = 0; i < 10; i++)
    {
        Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(materialList[i]);
        if (mp.isNull())
            OutputDebugString("problems ahead\n");
        Ogre::TextureUnitState* tus = mp->getTechnique(0)->getPass(0)->getTextureUnitState("env_map");
        if (tus == NULL)
            OutputDebugString("uh oh\n");
        tus->setTextureName(newSphereMap);
    }
#endif
}


void SimpleCoupeCar::loadDestroyedModel (void)
{
    mChassisNode->detachAllObjects();
    createGeometry("CarEntity_Burnt", "banger_burnt.mesh", mChassisNode, false);
    makeBitsFallOff();

#ifdef COLLISION_DOMAIN_CLIENT
    if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)            
        GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(), 3);
#endif
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void SimpleCoupeCar::initBody(Ogre::Vector3 carPosition)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("BangerCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "banger_collision.mesh");
    entity->setDebugDisplayEnabled( false );

    btVector3 inertia;
    btCompoundShape *compoundChassisShape = (btCompoundShape*) GameCore::mPhysicsCore->getCollisionShape( PHYS_SHAPE_BANGER );
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

void SimpleCoupeCar::removeFLDoor() {
    removePiece( mFLDoorNode, mFLDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  0.773f, 0.895f, 0.315f ) );
}
void SimpleCoupeCar::removeFRDoor() {
    removePiece( mFRDoorNode, mFRDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  -0.773f, 0.895f, 0.315f ) );
}
void SimpleCoupeCar::removeRLDoor() {
    removePiece( mRLDoorNode, mRLDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  0.773f, 0.900f, 0.679f ) );
}
void SimpleCoupeCar::removeRRDoor() {
    removePiece( mRRDoorNode, mRRDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  -0.773f, 0.900f, 0.679f ) );
}
void SimpleCoupeCar::removeRBumper() {
    removePiece( mRBumperNode, mRBumperBody, PHYS_SHAPE_BANGER_RBUMPER, btVector3( 0.0f, 0.518f, -2.424f ) );
}
void SimpleCoupeCar::removeFBumper() {
    removePiece( mFBumperNode, mFBumperBody, PHYS_SHAPE_BANGER_RBUMPER, btVector3( 0.0f, 0.518f, 2.424f ) );
}

void SimpleCoupeCar::removeCarPart(unsigned int part) {
    switch(part) {
        case 0:
            removeFBumper();
            break;
        case 1:
            removeFBumper();
            break;
        case 2:
            ((float)rand()/RAND_MAX) > 0.5f ? removeFLDoor() : removeFRDoor();
            break;
        case 3:
             ((float)rand()/RAND_MAX) > 0.5f ? removeRLDoor() : removeRRDoor();
            break;
        case 4:
        case 5:
            removeRBumper();
            break;
    }
}

void SimpleCoupeCar::makeBitsFallOff()
{
    //mBodyNode->removeChild( "FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID) );
    removePiece( mFLDoorNode, mFLDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  0.773f, 0.895f, 0.315f ) );
    removePiece( mFRDoorNode, mFRDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3( -0.773f, 0.895f, 0.315f ) );
    removePiece( mRLDoorNode, mRLDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3(  0.773f, 0.900f, 0.679f ) );
    removePiece( mRRDoorNode, mRRDoorBody, PHYS_SHAPE_BANGER_DOOR, btVector3( -0.773f, 0.900f, 0.679f ) );

    removePiece( mFBumperNode, mFBumperBody, PHYS_SHAPE_BANGER_FBUMPER, btVector3( 0.0f, 0.486f,  2.424f ) );
    removePiece( mRBumperNode, mRBumperBody, PHYS_SHAPE_BANGER_RBUMPER, btVector3( 0.0f, 0.518f, -2.424f ) );
}
