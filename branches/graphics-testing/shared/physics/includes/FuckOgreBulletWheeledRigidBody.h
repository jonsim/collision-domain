/**
 * @file	
 * @brief 	That just cost me 4 hours. This file is nearly 100% copied and pasted 
            from OgrebulletDynamicsRigidBody.cpp (contains wheeled version too)
 */
#ifndef FUCKOGREBULLETWHEELEDRIGIDBODY_H
#define FUCKOGREBULLETWHEELEDRIGIDBODY_H

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace OgreBulletDynamics;

class FuckOgreBulletWheeledRigidBody : public RigidBody
{
public:
    FuckOgreBulletWheeledRigidBody(const Ogre::String &name, DynamicsWorld *world, const short collisionGroup, const short collisionMask): 
        RigidBody(name, world, collisionGroup, collisionMask),
            mVehicle(0)
    {
    };

        
    void setVehicle(RaycastVehicle *v){mVehicle = v;};

    virtual void setPosition(const btVector3 &pos);
    virtual void setOrientation(const btQuaternion &quat);
    virtual void setTransform(const btVector3 &pos, const btQuaternion &quat);
    virtual void setTransform(const btTransform& worldTrans);

    virtual ~FuckOgreBulletWheeledRigidBody(){};

protected:
	RaycastVehicle *mVehicle;
};

#endif // #ifndef FUCKOGREBULLETWHEELEDRIGIDBODY_H

