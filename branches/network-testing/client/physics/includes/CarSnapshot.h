/**
 * @file	CarSnapshot.h
 * @brief 	Contains all the required parameters to reliably "teleport" a car to a certain position.
                If you take a CarSnapshot it can later be restored by the interface in the Car class.
 */
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

    CarSnapshot(
        const btVector3 &position,
        const btQuaternion &rotation,
        const btVector3 &angularVelocity,
        const btVector3 &linearVelocity,
        const float &wheelPosition)
    {
        mPosition = position;
        mRotation = rotation;
        mAngularVelocity = angularVelocity;
        mLinearVelocity = linearVelocity;
        mWheelPosition = wheelPosition;
    };
};

#endif // #ifndef __CarSnapshot_h_

