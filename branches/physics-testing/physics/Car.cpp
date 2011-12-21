

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
        //mBodyNode->setPosition(0,20,0);
        //mBodyNode->needUpdate(true);
        //compoundChassisShape->getBulletShape->
        //mVehicle->setTransform();
        //OutputDebugString("Hello World");
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
