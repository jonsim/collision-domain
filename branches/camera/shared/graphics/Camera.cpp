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

    Ogre::Vector3 lookatPos  = pos + up * mLookOffset.getY()  + fwd * mLookOffset.getZ();

    mCam->lookAt( lookatPos );    
}

void GameCamera::update()
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

    // Sweep around the camera for objects we might collide with
    btSphereShape camSphere( 0.2f );
    btTransform cameraFrom, cameraTo;

    // Original camera pos = current pos
    cameraFrom.setIdentity();
    cameraFrom.setOrigin( mWorldPos );

    // Now move towards new position
    if( mCamType == CAM_CHASE )
        mWorldPos = mWorldPos.lerp( BtOgre::Convert::toBullet( desiredPos ), mTension );
    else if( mCamType == CAM_FIXED )
        mWorldPos = BtOgre::Convert::toBullet( desiredPos );

    cameraTo.setIdentity();
    cameraTo.setOrigin( mWorldPos );

    btCollisionWorld::ClosestConvexResultCallback cb( 
        cameraFrom.getOrigin(), cameraTo.getOrigin() );

    cb.m_collisionFilterMask = btBroadphaseProxy::AllFilter;

    GameCore::mPhysicsCore->getWorld()->convexSweepTest( &camSphere, cameraFrom, cameraTo, cb );

    if( cb.hasHit() )
    {
        btScalar minFraction = cb.m_closestHitFraction;
        mWorldPos.setInterpolate3( cameraFrom.getOrigin(), cameraTo.getOrigin(), minFraction );
    }

    mCam->setPosition( BtOgre::Convert::toOgre( mWorldPos ) );
    mCam->lookAt( lookatPos );
}