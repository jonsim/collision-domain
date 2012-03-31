/**
 * @file    Camera.h
 * @brief     Contains the controls for implementing various cameras
 * @details Tried to keep this as abstracted as possible - nothing specific to players,
            just a scene node to follow if you wish
 */
#ifndef CAMERA_H_
#define CAMERA_H_

#include "stdafx.h"
#include "SharedIncludes.h"

using namespace Ogre;

enum CAM_TYPE : int
{
    CAM_FIXED,
    CAM_CHASE,
    CAM_FREE,
};

class GameCamera
{
public:

    GameCamera( Camera* cam ) : 
        mCam(cam), mCamType(CAM_FREE), mTarget(NULL),
        mTension(0.2f), totalXRot(0.f), totalYRot(0.f)              {}

    void            setCamType( CAM_TYPE t )                        { mCamType = t; }
    CAM_TYPE        getCamType()                                    { return mCamType; }

    float           getTension()                                    { return mTension; }
    void            setTension( float t )                           { mTension = t; }

    bool            isCollidable()                                  { return mCollidable; }
    void            setCollidable( bool canCollide )                { mCollidable = canCollide; }

    SceneNode&      getTarget()                                     { return *mTarget; }
    void            setTarget( SceneNode *node )                    { if( node ) mTarget = node; }
    void            removeTarget()                                  { mTarget = NULL; }

    btVector3&      getTransform()                                  { return mWorldPos; }
    void            setTransform( btVector3& t )                    { mWorldPos = t; }

    btVector3&      getOffset()                                     { return mLocalOffset; }
    void            setOffset( btVector3& offset )                  { mLocalOffset = offset; }

    btVector3&      getLookOffset()                                 { return mLookOffset; }
    void            setLookOffset( btVector3& offset )              { mLookOffset = offset; }

    void            lookAtTarget();

    void            update( btScalar timeStep );
    void            update( Degree xRot, Degree yRot );

private:

    CAM_TYPE        mCamType;
    SceneNode*      mTarget;
    float           mTension;
    bool            mCollidable;

    btVector3       mWorldPos;
    btVector3       mLocalOffset;
    btVector3       mLookOffset;

    Camera*         mCam;

    Degree          totalXRot;
    Degree          totalYRot;
    

};


#endif // #ifndef CAMERA_H_