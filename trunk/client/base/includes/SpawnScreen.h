#ifndef SPAWNSCREEN_H
#define SPAWNSCREEN_H

#include "stdafx.h"
#include "SharedIncludes.h"

enum
{
    CAR_BANGER,
    CAR_SMALL,
    CAR_TRUCK,

    CAR_COUNT, // Num of car types
};

class SpawnScreen
{
private:
    Car *mViewCar[CAR_COUNT];
    int mCurrentCar;
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
};

#endif // ifndef SPAWNSCREEN_H