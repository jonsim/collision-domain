
#ifndef __Car_h_
#define __Car_h_

#include "stdafx.h"

class Car
{
public:
    virtual OgreBulletDynamics::RaycastVehicle *getVehicle() = 0;
    virtual Ogre::SceneNode *attachCamNode() = 0;
};

#endif // #ifndef __Car_h_

