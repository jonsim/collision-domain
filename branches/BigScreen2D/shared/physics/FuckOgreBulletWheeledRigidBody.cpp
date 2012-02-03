/**
 * @file	
 * @brief 	That just cost me 4 hours. This file is 100% copied and pasted
            from OgrebulletDynamicsRigidBody.cpp (contains wheeled version too)
 */
#include "stdafx.h"
#include "SharedIncludes.h"

void FuckOgreBulletWheeledRigidBody::setPosition(const btVector3 &pos)
{ 
    //should update wheels as well ?
    mRootNode->setPosition(pos[0], pos[1], pos[2]);
};
// -------------------------------------------------------------------------
void FuckOgreBulletWheeledRigidBody::setOrientation(const btQuaternion &quat)
{ 
    mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());
};
// -------------------------------------------------------------------------
void FuckOgreBulletWheeledRigidBody::setTransform(const btVector3 &pos, const btQuaternion &quat)
{
    mRootNode->setPosition(pos[0], pos[1], pos[2]);
    mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());

    mVehicle->setTransform();
}
// -------------------------------------------------------------------------
void FuckOgreBulletWheeledRigidBody::setTransform(const btTransform& worldTrans)
{ 
    mRootNode->setPosition(worldTrans.getOrigin()[0], worldTrans.getOrigin()[1],worldTrans.getOrigin()[2]);
    mRootNode->setOrientation(worldTrans.getRotation().getW(),worldTrans.getRotation().getX(), worldTrans.getRotation().getY(), worldTrans.getRotation().getZ());

    mVehicle->setTransform();
}