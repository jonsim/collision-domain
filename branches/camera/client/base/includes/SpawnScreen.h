#ifndef SPAWNSCREEN_H
#define SPAWNSCREEN_H

#include "stdafx.h"
#include "SharedIncludes.h"


class SpawnScreen
{
private:
    Car *mViewCar[CAR_COUNT];
    CarType mCurrentCar;
    bool bSpawned;

    Ogre::Camera *mCam;
    Ogre::Vector3 mOrigCamPos;

public:
    SpawnScreen( Ogre::Camera* cam );
    ~SpawnScreen();

    void showCar();
    void hideCar();
    void switchCar( int direction );
    void selectCar();
    Car *getCar();
};

#endif // ifndef SPAWNSCREEN_H