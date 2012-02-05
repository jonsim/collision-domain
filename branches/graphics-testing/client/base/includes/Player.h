/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "CarCam.h"

/*-------------------- CLASS DEFINITIONS --------------------*/
/// CarType represents the type of car used as the player object (i.e. the model).
//enum CarType {SMALL, MEDIUM, LARGE};

/// CarSkin represents the texture applied to the player object.
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

enum CarType : int
{
    CAR_BANGER = 0,
    CAR_SMALL,
    CAR_TRUCK,

    CAR_COUNT, // Num of car types
};

/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class Player //: public PhysicsEntity
{
 
public:
    Player ();
    ~Player ();
    void createPlayer (Ogre::SceneManager* sm, CarType iCarType, CarSkin s, PhysicsCore *physicsCore);
    void attachCamera (Ogre::Camera* cam);
    void processControlsFrameEvent(
        InputState *userInput,
        Ogre::Real secondsSinceLastFrame,
        float targetPhysicsFrameRate);
    void updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth);
	float getCameraYaw ();
    Car* getCar();
    void collisionTickCallback(int damage);
	
	CarSnapshot *mSnapshots;
    
    const char *getNickname() { return mNickname; }
    int getCarType() { return mCarType; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname( char *szNick ) { mNickname = strdup( szNick ); }

private:
    const float cameraRotationConstant;

    Car *mCar;
    CarSnapshot *mCarSnapshot;
    char *mNickname;
	CarCam*	mCarCam;
	Ogre::SceneNode *camNode;
	Ogre::SceneNode *camArmNode;

    CarType mCarType;
};

#endif // #ifndef PLAYER_H
