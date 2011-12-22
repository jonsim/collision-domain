

#include "stdafx.h"
#include "Car.h"


/* TODO - steering acceleration needs to take into account timesincelastframe */
void Car::steerInputTick(bool isLeft, bool isRight)
{
    // process steering on both wheels (+1 = left, -1 = right)
    int leftRight = 0;
    if (isLeft)  leftRight += 1;
    if (isRight) leftRight -= 1;

    // we don't want to go straight to this steering value (i.e. apply acceleration to steer value)
    if (leftRight != 0) mSteer += mSteerIncrement * leftRight;
    else
    {
        // go back to zero
        if (mSteer >= 0) mSteer = mSteer >= mSteerToZeroIncrement ? mSteer - mSteerToZeroIncrement : 0.0f;
        else mSteer = mSteer <= -mSteerToZeroIncrement ? mSteer + mSteerToZeroIncrement : 0.0f;
    }
    

    //Ogre::Vector3 x = mBodyNode->getPosition();
        //OutputDebugString("Hello World1");
    if (isLeft && isRight) {
        //mCarChassis->getBulletRigidBody()->translate(btVector3(90.0f,5.00f,0.0f));

        btVector3 axis(0,1,0);
        btScalar angle(0.0f);
        btQuaternion quat(axis, angle);
        btTransform transform(quat, btVector3(90.0f,2.00f,0.0f));
        mCarChassis->getBulletRigidBody()->proceedToTransform(transform);
        //mCarChassis->getBulletRigidBody()->setWorldTransform(transform);

        // The car will now be moved and rotated as specified, but the current impulse
        // will be pointing in the wrong direction (if rotation is now different).
        /*mCarChassis->getBulletRigidBody()->applyCentralForce;
        mCarChassis->getBulletRigidBody()->applyCentralImpulse;
        mCarChassis->getBulletRigidBody()->applyCentralImpulse;
        mCarChassis->getBulletRigidBody()->applyImpulse;
        mCarChassis->getBulletRigidBody()->getAngularFactor;
        mCarChassis->getBulletRigidBody()->getAngularVelocity;
        mCarChassis->getBulletRigidBody()->getDeltaAngularVelocity;
        mCarChassis->getBulletRigidBody()->getDeltaLinearVelocity;
        mCarChassis->getBulletRigidBody()->getInvInertiaDiagLocal;
        mCarChassis->getBulletRigidBody()->getInvInertiaTensorWorld;
        mCarChassis->getBulletRigidBody()->getLinearFactor;
        mCarChassis->getBulletRigidBody()->getLinearVelocity;
        mCarChassis->getBulletRigidBody()->getPushVelocity;
        mCarChassis->getBulletRigidBody()-;
        mCarChassis->getBulletRigidBody()->setAngularFactor;
        mCarChassis->getBulletRigidBody()->setAngularVelocity;
        mCarChassis->getBulletRigidBody()->setInvInertiaDiagLocal;
        mCarChassis->getBulletRigidBody()->setLinearFactor;
        mCarChassis->getBulletRigidBody()->setLinearVelocity;
        mCarChassis->getBulletRigidBody()->updateInertiaTensor;*/



        
        //mBodyNode->setVisible(false); 
    //float x = mVehicle->getBulletVehicle()->getCurrentSpeedKmHour();
    //btVector3 axis(0,1,0);

    //OgreBulletCollisions::CompoundCollisionShape *compoundChassisShape;
    //OgreBulletDynamics::WheeledRigidBody         *mCarChassis;
    //OgreBulletDynamics::VehicleRayCaster         *mVehicleRayCaster;
    //OgreBulletDynamics::RaycastVehicle           *mVehicle;
        
        
        
        //mCarChassis->setVisible(false);
        //mCarChassis->setTransform(btTransform(quat, btVector3(80.0f, 20.0f, 80.0f)));
        //mCarChassis->setVisible(true);

        //mCarChassis->


    //xyz, wxyz
   // mCar->mPlayerNode->
    //Ogre::Quaternion(
    //Ogre::Quaternion(Ogre::Radian(newState.getRotation()), Ogre::Vector3::UNIT_Y)


        //mBodyNode->setPosition(0,20,0);
        //mBodyNode->needUpdate(true);
        //mVehicle->setTransform();

        //mCarChassis->setPosition(btVector3(80.0f, 20.0f, 80.0f));
        //mCarChassis->getBulletRigidBody(); // no setposition
        //mCarChassis->getBulletObject(); // no setpositin
        //mVehicleRayCaster->getBulletVehicleRayCaster(); // no setposition
        //btScalar deltaTimeStep = 0.001;
        //mVehicle->getBulletActionInterface()->updateAction(mWorld->getBulletCollisionWorld(), deltaTimeStep);
        //mVehicle->getBulletVehicle()->getChassisWorldTransform();


        //mVehicle->getBulletVehicle()->getChassisWorldTransform().setOrigin(btVector3(80.0f, 20.0f, 80.0f));

        //compoundChassisShape->getBulletShape->
        //mVehicle->getBulletVehicle()->updateVehicle(0.001);
        //mVehicle->setTransform();
        //OutputDebugString("Hello World");
        //mVehicle->getBulletActionInterface()->
        //mChassis-
        //Ogre::Vector3 x = mBodyNode->getPosition();
        //OutputDebugString("Hello World2");
    }


    // don't steer too far! Use the clamps.
    mSteer = mSteer > mSteerClamp ? mSteerClamp : (mSteer < -mSteerClamp ? -mSteerClamp : mSteer);

    mVehicle->setSteeringValue(mSteer, 0);
    mVehicle->setSteeringValue(mSteer, 1);
}


void Car::accelInputTick(bool isForward, bool isBack)
{
    int forwardBack = 0;
    if (isForward) forwardBack += 1;
    if (isBack)    forwardBack -= 1;

    float f = forwardBack < 0 ? mMaxBrakeForce : mMaxAccelForce; 

    mVehicle->applyEngineForce(f * forwardBack, 0);
    mVehicle->applyEngineForce(f * forwardBack, 1);
}


/// @brief  If a node isnt already attached, attaches a new one, otherwise returns the current one
/// @return The node onto which a camera can be attached to observe the car.
Ogre::SceneNode *Car::attachCamNode()
{
    if (mCamNode != NULL) return mCamNode;

    // else we need to make a new camera
    mCamArmNode = mBodyNode->createChildSceneNode("CamArmNode" + boost::lexical_cast<std::string>(mUniqueCarID));
    mCamNode = mCamArmNode->createChildSceneNode("CamNode" + boost::lexical_cast<std::string>(mUniqueCarID));

    return mCamNode;
}


void Car::createGeometry(
                    const std::string &entityName,
                    const std::string &meshName,
                    Ogre::SceneNode *toAttachTo)
{
    createGeometry(entityName, meshName, false, "", toAttachTo);
}


void Car::createGeometry(
                    const std::string &entityName,
                    const std::string &meshName,
                    const std::string &materialName,
                    Ogre::SceneNode *toAttachTo)
{
    createGeometry(entityName, meshName, true, materialName, toAttachTo);
}


void Car::createGeometry(
                    const std::string &entityName,
                    const std::string &meshName,
                    bool applyMaterial,
                    const std::string &materialName,
                    Ogre::SceneNode *toAttachTo)
{
    Ogre::Entity* entity;
    entity = mSceneMgr->createEntity(entityName + boost::lexical_cast<std::string>(mUniqueCarID), meshName);
    if (applyMaterial) entity->setMaterialName(materialName);

    int GEOMETRY_QUERY_MASK = 1<<2;
    entity->setQueryFlags(GEOMETRY_QUERY_MASK); // lets raytracing hit this object (for physics)
#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    //DOESNT WORKmSceneMgr->setFlipCullingOnNegativeScale(false); // make sure that the culling mesh gets flipped for negatively scaled nodes
    entity->setCastShadows(true);
    toAttachTo->attachObject(entity);
}
