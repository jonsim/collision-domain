
#ifndef __CarSnapshot_h_
#define __CarSnapshot_h_

#include "stdafx.h"

class CarSnapshot
{
public:
    // Car location and rotation
    btVector3 mPosition;
    btQuaternion mRotation;
    btVector3 mAngularVelocity;
    btVector3 mLinearVelocity;

    // Car steering, engine and brake
    float mWheelPosition;
    float mEngineForce;

    CarSnapshot(
        const btVector3 &position,
        const btQuaternion &rotation,
        const btVector3 &angularVelocity,
        const btVector3 &linearVelocity,

        const float &wheelPosition,
        const float &engineForce)
    {
        mPosition = position;
        mRotation = rotation;
        mAngularVelocity = angularVelocity;
        mLinearVelocity = linearVelocity;

        mWheelPosition = wheelPosition;
        mEngineForce = engineForce;
    };
};

#endif // #ifndef __CarSnapshot_h_

