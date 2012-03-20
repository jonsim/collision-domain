/**
 * @file	Camera.cpp
 * @brief 	Contains the controls for implementing various cameras
 * @details Tried to keep this as abstracted as possible - nothing specific to players,
            just a physics object to follow if so desired.
 */

#include "stdafx.h"
#include "SharedIncludes.h"

void GameCamera::lookAtTarget()
{
    if( !mTarget )
        return;

    // Get the position of the object to follow
    Ogre::Vector3 pos   =  mTarget->getPosition();
    Ogre::Vector3 up    =  mTarget->getLocalAxes().GetColumn(1);
    Ogre::Vector3 fwd   =  mTarget->getLocalAxes().GetColumn(2);

    up.normalise();
    fwd.normalise();

    // Add on the offset for the position to focus on
    Ogre::Vector3 lookatPos  = pos + up * mLookOffset.getY()  + fwd * mLookOffset.getZ();

    mCam->lookAt( lookatPos );    
}

void GameCamera::update( btScalar timeStep )
{
    // Fixed camera
    if( mCamType == CAM_FREE )
    {
        // Don't move, just look towards target if there is one
        if( mTarget != NULL )
            lookAtTarget();

        return;
    }

    // Following camera, target not set yet
    if( !mTarget )
        return;

    // Get the position of the object to follow
    Ogre::Vector3 pos   =  mTarget->getPosition();
    Ogre::Vector3 up    =  mTarget->getLocalAxes().GetColumn(1);
    Ogre::Vector3 fwd   =  mTarget->getLocalAxes().GetColumn(2);

    up.normalise();
    fwd.normalise();

    // Add on the camera offset to the local axes
    Ogre::Vector3 desiredPos = pos + up * mLocalOffset.getY() + fwd * mLocalOffset.getZ();
    // Adjust the position to look at on the local axes
    Ogre::Vector3 lookatPos  = pos + up * mLookOffset.getY()  + fwd * mLookOffset.getZ();

    // Set up some transforms in if we need to check for collisions
    btTransform cameraFrom, cameraTo;

    // Original camera pos = current pos
    cameraFrom.setIdentity();
    cameraFrom.setOrigin( mWorldPos );

    // Now move towards new position
    if( mCamType == CAM_CHASE )
        mWorldPos = mWorldPos.lerp( BtOgre::Convert::toBullet( desiredPos ), timeStep * mTension );
    else if( mCamType == CAM_FIXED )
        mWorldPos = BtOgre::Convert::toBullet( desiredPos );

    // Test for collisions if the camera is collidable
    if( mCollidable )
    {

        // Set up the position the camera wants to move to
        cameraTo.setIdentity();
        cameraTo.setOrigin( mWorldPos );

        // Create a physics shape for the camera
        btSphereShape camSphere( 0.2f );

        // Set up collision detection callback
        btCollisionWorld::ClosestConvexResultCallback cb( cameraFrom.getOrigin(), cameraTo.getOrigin() );
        // Set to collide with everything (includes arena and cars then (and actually maybe pickups, might need to change))
        cb.m_collisionFilterMask = btBroadphaseProxy::AllFilter;

        // Sweep test for objects
        GameCore::mPhysicsCore->getWorld()->convexSweepTest( &camSphere, cameraFrom, cameraTo, cb );

        if( cb.hasHit() )
        {
            // If we hit something, interpolate beteween old cam pos and furthest possible distsnce before hit
            btScalar minFraction = cb.m_closestHitFraction;
            mWorldPos.setInterpolate3( cameraFrom.getOrigin(), cameraTo.getOrigin(), minFraction );
        }
    }

    // Update the camera position
    mCam->setPosition( BtOgre::Convert::toOgre( mWorldPos ) );
    // Turn to face target object
    mCam->lookAt( lookatPos );
}

void GameCamera::update( Degree xRot, Degree yRot )
{
    // Special update method for fixed cameras so we can rotate around object
    if( mCamType != CAM_FIXED )
        return;

    // Following camera, target not set yet
    if( !mTarget )
        return;

    totalXRot += xRot;
    if( abs( totalXRot.valueDegrees() ) >= 180 )
        totalXRot = -totalXRot;
    if( totalXRot.valueDegrees() >= 360 )
        totalXRot -= Ogre::Degree(360);
    if( totalXRot.valueDegrees() <= -360 )
        totalXRot += Ogre::Degree(360);

    totalYRot += yRot;
    if( abs( totalYRot.valueDegrees() ) >= 180 )
        totalYRot = -totalYRot;
    if( totalYRot.valueDegrees() >= 360 )
        totalYRot -= Ogre::Degree(360);
    if( totalYRot.valueDegrees() <= -360 )
        totalYRot += Ogre::Degree(360);

    // Get the position of the object to follow
    Ogre::Vector3 pos   =  mTarget->getPosition();
    Ogre::Vector3 up    =  Ogre::Quaternion( 1.f, totalYRot.valueRadians(), 0.f, 0.f ) * mTarget->getLocalAxes().GetColumn(1);
    Ogre::Vector3 fwd   =  Ogre::Quaternion( 1.f, 0.f, totalXRot.valueRadians(), 0.f ) * mTarget->getLocalAxes().GetColumn(2);

    up.normalise();
    fwd.normalise();

    // Add on the camera offset to the local axes
    Ogre::Vector3 desiredPos = pos + up * mLocalOffset.getY() + fwd * mLocalOffset.getZ();

    //Ogre::Quaternion rot( 1.f, totalYRot.valueRadians(), totalXRot.valueRadians(), 0.f );
    //desiredPos = rot * desiredPos;

    mWorldPos = BtOgre::Convert::toBullet( desiredPos );

    // Update the camera position
    mCam->setPosition( BtOgre::Convert::toOgre( mWorldPos ) );

    // Turn to face target object
    mCam->lookAt( pos );
}