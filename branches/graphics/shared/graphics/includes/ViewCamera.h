/**
* @file        ViewCamera.h
* @brief    Holds a pointer to a camera used for views in the server. And holds aditional info

*/
#ifndef VIEWCAMERA_H
#define VIEWCAMERA_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "RakNetTypes.h"
#include "GetTime.h"

class ViewCamera
{
public:
                        ViewCamera(Ogre::Camera* cam);
    Ogre::Camera*        getCamera();
    RakNet::TimeMS        getLastUpdateTime();
    void                setLastUpdateTime(RakNet::TimeMS lastUpdate);
    void                setLastUpdateTime();
private:
    RakNet::TimeMS        mLastUpdate;
    Ogre::Camera*        mCam;
};

#endif