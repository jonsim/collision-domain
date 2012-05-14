/**
 * @file	SimpleCoupeCar.cpp
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#include "stdafx.h"
#include "PhysicsCore.h"
#include "SmallCar.h"
#include "GameCore.h"
#include "Gameplay.h"

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

#define SMALLCAR_VTX_COUNT 20

/*static btScalar SmallCarVtx[] = {
    -0.46733353f, 	-0.50000006f, 	0.47412637f,
	0.46733356f, 	-0.50000006f, 	0.47412637f,
	-0.46733353f, 	0.31162921f, 	0.47412637f,
	0.46733356f, 	0.31162921f, 	0.47412637f,
	-0.5f, 			0.46859881f, 	0.19854486f,
	0.5f, 			0.46859881f, 	0.19854486f,
	-0.47566596f, 	-0.50000006f, 	-0.69251001f,
	0.47566596f, 	-0.50000006f, 	-0.69251001f,
	0.0f, 			-0.50000006f, 	0.50817901f,
	0.0f, 			0.26104462f, 	0.5016675f,
	0.0f, 			0.61385989f, 	0.19854486f,
	0.0f, 			0.46093348f, 	0.43777251f,
	0.47566596f, 	0.16506362f, 	-0.68763894f,
	-0.47566596f, 	0.16506362f, 	-0.68763894f,
};*/



static btScalar SmallCarVtx[] = {
    -62.4181f, -32.3895f, 125.804f,
    62.3984f, -32.3895f, 125.804f,
    -62.4181f, 20.187f, 125.804f,
    62.3984f, 20.187f, 125.804f,
    -66.7804f, 30.3553f, 52.6817f,
    66.7607f, 30.3553f, 52.6817f,
    -63.5308f, -32.3895f, -183.75f,
    63.5112f, -32.3895f, -183.75f,
    -0.00984f, -32.3895f, 134.84f,
    -0.00984f, 16.9102f, 133.112f,
    -0.00984f, 39.7652f, 52.6817f,
    -0.00984f, 29.8588f, 116.158f,
    55.813f, 73.7734f, 20.3985f,
    63.5112f, 10.6926f, -182.458f,
    -63.5308f, 10.6926f, -182.458f,
    -55.8327f, 73.7734f, 20.3985f,
    -0.00984f, 81.6404f, 20.3985f,
    51.076f, 71.9659f, -110.318f,
    -51.0957f, 71.9659f, -110.318f,
    -0.00985f, 78.5039f, -110.318f,
};

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
    mMaxSuspensionForce     =   6000.0f;
    mFrictionSlip           =   4.0f;
	mChassisLinearDamping   =   0.2f;
	mChassisAngularDamping  =   0.2f;
	mChassisRestitution		=   0.6f;
	mChassisFriction        =   0.6f;
	mChassisMass            = 1000.0f;//585.0f;

    mWheelRadius            =  0.2775f; // this is actually diameter!!
    mWheelWidth             =  0.153f;
    mWheelFriction          =  2.2f;    //1000;//1e30f;
    mConnectionHeight       =  0.3f;    // this connection point lies at the very bottom of the suspension travel
    
    mSteerIncrement         =  0.015f;
    mSteerToZeroIncrement   =  0.05f;   // when no input is given steer back to 0
    mSteerClamp             =  0.75f;

    mMaxAccelForce = 4000.0f;
    mMaxBrakeForce = 300.0f;
    mMaxAccelForceBuf = mMaxAccelForce;

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
SmallCar::SmallCar(int uniqueCarID, TeamID tid, ArenaID aid) : Car(uniqueCarID),
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
    mHornSound = GameCore::mAudioCore->getSoundInstance(HORN_HIGH, mUniqueCarID, NULL);
    
    // pitch is in play rate increase (4x max) (100 = 3.976x play rate)
    mEngineSound = GameCore::mAudioCore->getSoundInstance(ENGINE_SMALL, mUniqueCarID, NULL, true);
    mEngineSound->setPitch(1.0f);
#endif
}


/// @brief  Destructor to clean up. Doesn't currently remove the car from the physics world though.
SmallCar::~SmallCar(void)
{
    GameCore::mPhysicsCore->removeBody( mCarChassis );
    GameCore::mPhysicsCore->removeBody( mLDoorBody );
    GameCore::mPhysicsCore->removeBody( mRDoorBody );
    GameCore::mPhysicsCore->removeBody( mFBumperBody );
    GameCore::mPhysicsCore->removeBody( mRBumperBody );

    GameCore::mPhysicsCore->getWorld()->removeAction( mVehicle );

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
    GameCore::mAudioCore->deleteSoundInstance(mHornSound);
    GameCore::mAudioCore->deleteSoundInstance(mEngineSound);
#endif
}

void SmallCar::createCollisionShapes()
{
    btCompoundShape* compoundChassisShape = new btCompoundShape();
    btTransform chassisShift( btQuaternion::getIdentity(), btVector3( 0, 0.55f, 0.0f ) );
    btConvexHullShape *convexHull = new btConvexHullShape( SmallCarVtx, SMALLCAR_VTX_COUNT, 3 * sizeof( btScalar ) );
    convexHull->setLocalScaling( btVector3( MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT, MESH_SCALING_CONSTANT ) );
    compoundChassisShape->addChildShape( chassisShift, convexHull );

    btBoxShape *doorShape = new btBoxShape( btVector3( 0.25f, 0.58f, 0.55f ) );
    btBoxShape *fBumperShape = new btBoxShape( btVector3( 0.773f, 0.25f, 0.25f ) );
    btBoxShape *rBumperShape = new btBoxShape( btVector3( 0.773f, 0.25f, 0.25f ) );

    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_SMALLCAR, compoundChassisShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_SMALLCAR_DOOR, doorShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_SMALLCAR_FBUMPER, fBumperShape );
    GameCore::mPhysicsCore->setCollisionShape( PHYS_SHAPE_SMALLCAR_RBUMPER, rBumperShape );
}

void SmallCar::louderLocalSounds() {
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


void SmallCar::startEngineSound() {
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
void SmallCar::updateAudioPitchFrameEvent()
{
#ifdef COLLISION_DOMAIN_CLIENT
    float maxPitch = 1.2f;

    updateRPM();
    float rpm = this->getRPM();
    // min 1800    max 7054

    rpm -= 1800;
    if (rpm < 0) rpm = 0;
    float pitch = (rpm / 5254.f);
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


void SmallCar::playCarHorn()
{
#ifdef COLLISION_DOMAIN_CLIENT
    GameCore::mAudioCore->playSoundOrRestart(mHornSound);
#endif
}


/// @brief  Initialises the node tree for this car.
void SmallCar::initNodes()
{
    // Player node.
    mPlayerNode = GameCore::mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    // Top level nodes.
    mBodyNode       = mPlayerNode->createChildSceneNode("BodyNode"   + boost::lexical_cast<std::string>(mUniqueCarID));
    mWheelsNode     = mPlayerNode->createChildSceneNode("WheelsNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    mChassisNode    = mBodyNode->createChildSceneNode("ChassisNode"     + boost::lexical_cast<std::string>(mUniqueCarID));
    mLDoorNode      = mBodyNode->createChildSceneNode("LDoorNode"       + boost::lexical_cast<std::string>(mUniqueCarID));
    mRDoorNode      = mBodyNode->createChildSceneNode("RDoorNode"       + boost::lexical_cast<std::string>(mUniqueCarID));
    mFBumperNode    = mBodyNode->createChildSceneNode("FBumperNode"     + boost::lexical_cast<std::string>(mUniqueCarID));
    mRBumperNode    = mBodyNode->createChildSceneNode("RBumperNode"     + boost::lexical_cast<std::string>(mUniqueCarID));
    mLHeadlightNode = mBodyNode->createChildSceneNode("mLHeadlightNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRHeadlightNode = mBodyNode->createChildSceneNode("mRHeadlightNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    
    // Wheel nodes.
    mFLWheelNode    = mWheelsNode->createChildSceneNode("FLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mFRWheelNode    = mWheelsNode->createChildSceneNode("FRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRLWheelNode    = mWheelsNode->createChildSceneNode("RLWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mRRWheelNode    = mWheelsNode->createChildSceneNode("RRWheelNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    // Scale all nodes.
    PhysicsCore::auto_scale_scenenode(mChassisNode);
    PhysicsCore::auto_scale_scenenode(mLDoorNode);
    PhysicsCore::auto_scale_scenenode(mRDoorNode);
    PhysicsCore::auto_scale_scenenode(mFBumperNode);
    PhysicsCore::auto_scale_scenenode(mRBumperNode);
    PhysicsCore::auto_scale_scenenode(mLHeadlightNode);
    PhysicsCore::auto_scale_scenenode(mRHeadlightNode);
    PhysicsCore::auto_scale_scenenode(mFLWheelNode);
    PhysicsCore::auto_scale_scenenode(mFRWheelNode);
    PhysicsCore::auto_scale_scenenode(mRLWheelNode);
    PhysicsCore::auto_scale_scenenode(mRRWheelNode);
}


/// @brief  Loads the car parts' meshes and attaches them to the (already initialised) nodes.
void SmallCar::initGraphics(TeamID tid, ArenaID aid)
{
    // Load the small car meshes.
	// true means it is deformable, therefore the unique entity name (defined in graphics code) needs to
	// be used in the first parameter
    createGeometry("UnIqUe_SmallCarBody",    "small_car_body.mesh",       mChassisNode,    true);
    createGeometry("UnIqUe_SmallCarLDoor",   "small_car_ldoor.mesh",      mLDoorNode,      true);
    createGeometry("UnIqUe_SmallCarRDoor",   "small_car_rdoor.mesh",      mRDoorNode,      true);
    createGeometry("UnIqUe_SmallCarFBumper", "small_car_fbumper.mesh",    mFBumperNode,    true);
    createGeometry("UnIqUe_SmallCarRBumper", "small_car_rbumper.mesh",    mRBumperNode,    true);
    createGeometry("CarEntity_LHeadlight",   "small_car_lheadlight.mesh", mLHeadlightNode, false);
    createGeometry("CarEntity_RHeadlight",   "small_car_rheadlight.mesh", mRHeadlightNode, false);
    createGeometry("CarEntity_FLWheel",      "small_car_lwheel.mesh",     mFLWheelNode,    false);
    createGeometry("CarEntity_FRWheel",      "small_car_rwheel.mesh",     mFRWheelNode,    false);
    createGeometry("CarEntity_RLWheel",      "small_car_lwheel.mesh",     mRLWheelNode,    false);
    createGeometry("CarEntity_RRWheel",      "small_car_rwheel.mesh",     mRRWheelNode,    false);
    
	// Setup particles.
    mExhaustSystem = GameCore::mSceneMgr->createParticleSystem("Exhaust" + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/SmallCar/Exhaust");
	mDustSystem    = GameCore::mSceneMgr->createParticleSystem("Dust"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Dust");
    mSmokeSystem   = GameCore::mSceneMgr->createParticleSystem("Smoke"   + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Smoke");
    mFireSystem    = GameCore::mSceneMgr->createParticleSystem("Fire"    + boost::lexical_cast<std::string>(mUniqueCarID), "CollisionDomain/Fire");
	mBodyNode->attachObject(mExhaustSystem);
	mBodyNode->attachObject(mDustSystem);
	mBodyNode->attachObject(mSmokeSystem);
	mBodyNode->attachObject(mFireSystem);

    // Place the exhasut emitter.
    //mExhaustSystem->getEmitter(0)->setPosition(Ogre::Vector3(-0.25f, 0.25f, -1.5f));
    // Place the dust emitters. These should be placed in the location of the wheel nodes but since
    // the wheels nodes are not currently positioned correctly these are hard coded numbers.
    mDustSystem->getEmitter(0)->setPosition(Ogre::Vector3( 0.6f, 0.2f,  1.1f));  // FL
    mDustSystem->getEmitter(1)->setPosition(Ogre::Vector3(-0.6f, 0.2f,  1.1f));  // FR
    mDustSystem->getEmitter(2)->setPosition(Ogre::Vector3( 0.6f, 0.2f, -1.1f));  // RL
    mDustSystem->getEmitter(3)->setPosition(Ogre::Vector3(-0.6f, 0.2f, -1.1f));  // RR
    // The smoke emitter should be placed over the engine.
    mSmokeSystem->getEmitter(0)->setPosition(Ogre::Vector3(0, 0, 0));
    // As should the fire emitter (which is, slightly more complex as it is a box).
    mFireSystem->getEmitter(0)->setParameter("width",  "2");
    mFireSystem->getEmitter(0)->setParameter("height", "2");    // height and depth are effectively switched
    mFireSystem->getEmitter(0)->setPosition(Ogre::Vector3(0.0f, 1.1f, 1.7f));

    // Update the skin based on the team
    updateTeam(tid);
    updateArena(aid);

    // The variables which aren't yet to be used <- what the hell are these?
    mCamArmNode  = NULL;
    mCamNode     = NULL;
}


void SmallCar::updateTeam (TeamID tid)
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
        setMaterial("small_car_body_uv",  mChassisNode);
        setMaterial("small_car_door_uv", mLDoorNode);
        setMaterial("small_car_door_uv", mRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),0);
        #endif
        break;
    case BLUE_TEAM:
        setMaterial("small_car_body_t1",  mChassisNode);
        setMaterial("small_car_door_t1", mLDoorNode);
        setMaterial("small_car_door_t1", mRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),1);
        #endif
        break;
    case RED_TEAM:
        setMaterial("small_car_body_t2",  mChassisNode);
        setMaterial("small_car_door_t2", mLDoorNode);
        setMaterial("small_car_door_t2", mRDoorNode);
        #ifdef COLLISION_DOMAIN_CLIENT
            if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
                GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),2);
        #endif
        break;
    default:
        break;
    }
}


void SmallCar::updateArena (ArenaID aid)
{
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

    // Update the environment map.
    std::string newSphereMap = (aid == COLOSSEUM_ARENA) ? "arena1_spheremap.jpg" : ((aid == FOREST_ARENA) ? "arena2_spheremap.jpg" : "arena3_spheremap.jpg");
    std::string materialList[7] = {"small_car_body_uv", "small_car_door_uv",
                                   "small_car_body_t1", "small_car_door_t1",
                                   "small_car_body_t2", "small_car_door_t2",
                                   "small_car_bumper"};
    for (unsigned char i = 0; i < 7; i++)
    {
        Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(materialList[i]);
        if (mp.isNull())
            OutputDebugString("problems ahead\n");
        Ogre::TextureUnitState* tus = mp->getTechnique(0)->getPass(0)->getTextureUnitState("env_map");
        if (tus == NULL)
            OutputDebugString("uh oh\n");
        tus->setTextureName(newSphereMap);
    }
}


void SmallCar::loadDestroyedModel (void)
{
    mChassisNode->detachAllObjects();
    createGeometry("CarEntity_Burnt", "small_car_burnt.mesh", mChassisNode, false);
    makeBitsFallOff();
    #ifdef COLLISION_DOMAIN_CLIENT
        if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)            
            GameCore::mClientGraphics->mBigScreen->changeArrow(this->getUniqueID(),3);
    #endif
}


/// @brief  Creates a physics car using the nodes (with attached meshes) and adds it to the physics world
void SmallCar::initBody(Ogre::Vector3 carPosition)
{
    // Load the collision mesh and create a collision shape out of it
    Ogre::Entity* entity = GameCore::mSceneMgr->createEntity("SmallCarCollisionMesh" + boost::lexical_cast<std::string>(mUniqueCarID), "small_car_collision.mesh");
    entity->setDebugDisplayEnabled( false );

    btVector3 inertia;
    btCompoundShape *compoundChassisShape = (btCompoundShape*) GameCore::mPhysicsCore->getCollisionShape( PHYS_SHAPE_SMALLCAR );
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


/// @brief  Attaches 4 wheels to the car chassis.
void SmallCar::initWheels()
{
    float wheelBaseLength = 1.0245f;
    float wheelBaseHalfWidth  = 0.5625f;

    // anything you add onto wheelbase, adjust this to take care of it
    float wheelBaseShiftZ = -0.17f;

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

void SmallCar::removeLDoor() {
    removePiece( mLDoorNode, mLDoorBody, PHYS_SHAPE_SMALLCAR_DOOR, btVector3(  0.594f, 0.788f, 0.011f ) );
}
void SmallCar::removeRDoor() {
    removePiece( mRDoorNode, mRDoorBody, PHYS_SHAPE_SMALLCAR_DOOR, btVector3(  -0.594f, 0.788f, 0.011f ) );
}
void SmallCar::removeRBumper() {
    removePiece( mRBumperNode, mRBumperBody, PHYS_SHAPE_SMALLCAR_RBUMPER, btVector3( 0.0f, 0.410f, -1.880f ) );
}
void SmallCar::removeFBumper() {
    removePiece( mFBumperNode, mFBumperBody, PHYS_SHAPE_SMALLCAR_FBUMPER, btVector3( 0.0f, 0.392f,  1.352f ) );
}

void SmallCar::removeCarPart(unsigned int part) {
    switch(part) {
        case 0:
#if _WIN32
        ((float)rand()/RAND_MAX) > 0.5f ? removeFBumper() : /* left headlamp */ 0;
            break;
        case 1:
            ((float)rand()/RAND_MAX) > 0.5f ? removeFBumper() : /* right headlamp */ 0;
#else
       ((float)rand()/RAND_MAX) > 0.5f ? removeFBumper() : /* left headlamp */ (void)0;
       	   break;
       case 1:
    	   ((float)rand()/RAND_MAX) > 0.5f ? removeFBumper() : /* right headlamp */ (void)0;
#endif
            break;
        case 2:
            removeLDoor();
            break;
        case 3:
            removeRDoor();
            break;
        case 4:
        case 5:
            removeRBumper();
            break;
    }
}

void SmallCar::makeBitsFallOff() {
    //mBodyNode->removeChild( "FLDoorNode"  + boost::lexical_cast<std::string>(mUniqueCarID) );
    removePiece( mLDoorNode, mLDoorBody, PHYS_SHAPE_SMALLCAR_DOOR, btVector3(  0.594f, 0.788f, 0.011f ) );
    removePiece( mRDoorNode, mRDoorBody, PHYS_SHAPE_SMALLCAR_DOOR, btVector3( -0.594f, 0.788f, 0.011f ) );

    removePiece( mFBumperNode, mFBumperBody, PHYS_SHAPE_SMALLCAR_FBUMPER, btVector3( 0.0f, 0.392f,  1.352f ) );
    removePiece( mRBumperNode, mRBumperBody, PHYS_SHAPE_SMALLCAR_RBUMPER, btVector3( 0.0f, 0.410f, -1.880f ) );
}
