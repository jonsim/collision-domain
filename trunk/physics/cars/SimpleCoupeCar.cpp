

#include "stdafx.h"
#include "cars/SimpleCoupeCar.h"

using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;


void SimpleCoupeCar::initTuning()
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
    
    mSteerIncrement = 0.025f;
    mSteerToZeroIncrement = 0.05f; // when no input is given steer back to 0
    mSteerClamp = 0.75f;

    mMaxAccelForce = 3000.0f;
    mMaxBrakeForce = 4000.0f;
}











SimpleCoupeCar::SimpleCoupeCar(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld *world, int uniqueCarID)
{
    mSceneMgr = sceneMgr;
    mWorld = world;
    mUniqueCarID = uniqueCarID;
    
    Ogre::Vector3 carPosition(16, 13, -15);
    Ogre::Vector3 chassisShift(0, 0.97, 0);

    initTuning();
    initNodes();
    initGraphics(chassisShift);
    initBody(carPosition, chassisShift);
    initWheels();
}


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


void SimpleCoupeCar::initNodes()
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


void SimpleCoupeCar::initGraphics(Ogre::Vector3 chassisShift)
{
    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    createGeometry("CarBody", "car2_body.mesh", "car2_body", mChassisNode);
    mChassisNode->scale(0.019, 0.019, 0.019);
   // mChassisNode->setPosition(chassisShift); - Doesn't work well with this mesh!!!

    // load the left door baby
    createGeometry("CarEntity_LDoor", "car2_door.mesh", "car2_door", mLDoorNode);
    mLDoorNode->scale(0.019, 0.019, 0.019);
    mLDoorNode->translate(43.0 * 0.019, 20.0 * 0.019, 22.0 * 0.019);
    
    // lets get a tasty right door
    createGeometry("CarEntity_RDoor", "car2_door.mesh", "car2_door", mRDoorNode);
    mRDoorNode->scale(-1, 1, 1);
    mRDoorNode->scale(0.019, 0.019, 0.019);
    mRDoorNode->translate(-46.0 * 0.019, 20.0 * 0.019, 22.0 * 0.019);

    // and now a sweet sweet front bumper
    createGeometry("CarEntity_FBumper", "car2_Fbumper.mesh", "car2_Fbumper", mFBumperNode);
    mFBumperNode->scale(0.019, 0.019, 0.019);
    mFBumperNode->translate(0, 20.0 * 0.019, 140.0 * 0.019);

    // and now a regular rear bumper
    createGeometry("CarEntity_RBumper", "car2_Rbumper.mesh", "car2_Rbumper", mRBumperNode);
    mRBumperNode->scale(-1, 1, 1);
    mRBumperNode->scale(0.019, 0.019, 0.019);
    mRBumperNode->translate(0, 20.0 * 0.019, -135.0 * 0.019);

    // tidy front left wheel
    createGeometry("CarEntity_FLWheel", "car2_wheel.mesh", "car2_wheel", mFLWheelNode);
    mFLWheelNode->scale(-1, 1, 1);
    mFLWheelNode->scale(0.019, 0.019, 0.019);

    // delightful front right wheel
    createGeometry("CarEntity_FRWheel", "car2_wheel.mesh", "car2_wheel", mFRWheelNode);
    mFRWheelNode->scale(0.019, 0.019, 0.019);

    // and now an arousing rear left wheel
    createGeometry("CarEntity_RLWheel", "car2_wheel.mesh", "car2_wheel", mRLWheelNode);
    mRLWheelNode->scale(-1, 1, 1);
    mRLWheelNode->scale(0.019, 0.019, 0.019);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry("CarEntity_RRWheel", "car2_wheel.mesh", "car2_wheel", mRRWheelNode);
    mRRWheelNode->scale(0.019, 0.019, 0.019);
    
    //Ogre::Entity *entity = mSceneMgr->createEntity("fag","car2_wheel.mesh");
    //const Ogre::AxisAlignedBox boundingBox = entity->getBoundingBox();
}


void SimpleCoupeCar::initBody(Ogre::Vector3 carPosition, Ogre::Vector3 chassisShift)
{
    // shift chassis collisionbox up chassisShift units above origin

    chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1.0197f, 0.63f, 2.6f));//Ogre::Vector3(1.f,0.75f,2.1f));

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

    mbtRigidBody = mCarChassis->getBulletRigidBody();
}


void SimpleCoupeCar::initWheels()
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

