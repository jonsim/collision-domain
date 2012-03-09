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
/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class Player
{
public:
    Player (void);
    ~Player (void);
    void createPlayer (CarType carType, CarSkin skin);
    void attachCamera (Ogre::Camera* cam);
    void processControlsFrameEvent (InputState *userInput, Ogre::Real secondsSinceLastFrame, float targetPhysicsFrameRate);
    void updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth);
	float getCameraYaw (void);
    Car* getCar (void);
    void collisionTickCallback (int damage);
    void applyHealthBonus (void);
    
    const char *getNickname (void) { return mNickname; }
    int getCarType (void) { return mCarType; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (char *szNick) { mNickname = strdup( szNick ); }
	int	 getHP (void);

    void killPlayer (void);

	CarSnapshot *mSnapshots;

private:
    const float      cameraRotationConstant;
	int              hp;
    char*            mNickname;
    Car*             mCar;
    CarSnapshot*     mCarSnapshot;
	CarCam*	         mCarCam;
	Ogre::SceneNode* camNode;
	Ogre::SceneNode* camArmNode;
    CarType          mCarType;
};

#endif // #ifndef PLAYER_H
