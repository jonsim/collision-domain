

#include "stdafx.h"
#include "Car.h"


void Car::restoreSnapshot(CarSnapshot *carSnapshot)
{
    moveTo(carSnapshot->mPosition, carSnapshot->mRotation);

    // After this the car will be moved and rotated as specified, but the current velocity
    // will be pointing in the wrong direction (if car is now rotated differently).
    mCarChassis->getBulletRigidBody()->setAngularVelocity(carSnapshot->mAngularVelocity);
    mCarChassis->getBulletRigidBody()->setLinearVelocity(carSnapshot->mLinearVelocity);


    carSnapshot->mEngineForce;

    mSteer = carSnapshot->mWheelPosition;
    applySteeringValue();
}


CarSnapshot *Car::getCarSnapshot()
{
    return new CarSnapshot(
        btVector3(mBodyNode->getPosition().x, mBodyNode->getPosition().y, mBodyNode->getPosition().z),
        mCarChassis->getBulletRigidBody()->getOrientation(),
        mCarChassis->getBulletRigidBody()->getAngularVelocity(),
        mCarChassis->getBulletRigidBody()->getLinearVelocity(),
        mSteer,
        mEngineForce);
}


void Car::moveTo(const btVector3 &position)
{
    moveTo(position, mCarChassis->getBulletRigidBody()->getOrientation());
}


/// This is PRIVATE for a reason. Without giving angular and linear velocity along with a new rotation
/// the rotation won't be applied nicely so the car will still keeo moving in the original direction
void Car::moveTo(const btVector3 &position, const btQuaternion &rotation)
{
    btTransform transform(rotation, position);
    mCarChassis->getBulletRigidBody()->proceedToTransform(transform);
    //mCarChassis->getBulletRigidBody()->setWorldTransform(transform);
}


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
    
    /*if (isLeft && isRight) {
        moveTo(
            btVector3(90.0f,2.00f,0.0f),
            btQuaternion(btVector3(0,1,0), btScalar(0.0f)));
    }*/

    applySteeringValue();
}


void Car::applySteeringValue()
{
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

    mEngineForce = f * forwardBack;

    mVehicle->applyEngineForce(mEngineForce, 0);
    mVehicle->applyEngineForce(mEngineForce, 1);

    //mVehicle->getBulletVehicle()->setBrake(1500.0f, 0);
    //mVehicle->getBulletVehicle()->setBrake(1500.0f, 1);
    //mVehicle->getBulletVehicle()->setBrake(1500.0f, 2);
    //mVehicle->getBulletVehicle()->setBrake(1500.0f, 3);
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
