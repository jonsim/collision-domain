#include "stdafx.h"
#include "GameIncludes.h"

SpawnScreen::SpawnScreen( Ogre::Camera* cam )
{
    mCam = cam;
    mOrigCamPos = cam->getPosition();

    // Initialize the car viewing array
    mViewCar[CAR_BANGER] = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID());
    mViewCar[CAR_SMALL] = (Car*) new SmallCar(GameCore::mPhysicsCore->getUniqueEntityID());
    mViewCar[CAR_TRUCK] = (Car*) new TruckCar(GameCore::mPhysicsCore->getUniqueEntityID());

    // Move the cars out of the way
    for( int i = 0; i < CAR_COUNT; i ++ )
    {
        mViewCar[i]->moveTo( btVector3((1200 + i*20), 0.5, 0) );
    }

    mCurrentCar = CAR_BANGER;
    showCar();

    bSpawned = false;
}

SpawnScreen::~SpawnScreen()
{
    // REMOVE THE CAR NODES...
    // ...WHICH SHOULD BE IN CAR.CPP WHY IS IT NOT?!

    for( int i = 0; i < CAR_COUNT; i ++ )
    {
        mViewCar[i]->moveTo( btVector3((1200 + i*20), 0.5, 0) );
        delete( mViewCar[i] );
    }

    mCam->setPosition( mOrigCamPos );
}

void SpawnScreen::showCar()
{
    mViewCar[mCurrentCar]->moveTo( btVector3(0,0.5,0) );
}

void SpawnScreen::hideCar()
{
    mViewCar[mCurrentCar]->moveTo( btVector3((1200 + mCurrentCar*20),0.5,0) );
}

void SpawnScreen::switchCar( int direction )
{
    hideCar();

    direction = direction <= 0 ? -1 : 1;
    int newCar = (int)mCurrentCar + direction;
    mCurrentCar = (CarType)newCar;
    if( mCurrentCar == -1 )
        mCurrentCar = (CarType)(CAR_COUNT - 1);
    if( mCurrentCar == CAR_COUNT )
        mCurrentCar = (CarType)0;

    showCar();
}

void SpawnScreen::selectCar()
{
    if( !bSpawned )
    {
        GameCore::mNetworkCore->sendSpawnRequest( mCurrentCar );
        bSpawned = true;
    }
}

Car *SpawnScreen::getCar()
{
    return mViewCar[mCurrentCar];
}
