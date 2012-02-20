/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


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
class Player
{
 
public:
    Player ();
    ~Player ();
    void createPlayer (CarType iCarType, CarSkin s);
    void attachCamera (Ogre::Camera* cam);
    void processControlsFrameEvent (InputState *userInput, Ogre::Real secondsSinceLastFrame, float targetPhysicsFrameRate);
    void updateCameraFrameEvent (int XRotation, int YRotation);
    Car* getCar (void);
    void collisionTickCallback (int damage);
    void applyHealthBonus (void);
        
    const char *getNickname (void) { return mNickname; }
    int getCarType (void) { return mCarType; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (char *szNick) { mNickname = strdup( szNick ); }
	int	 getHP (void);
	
	void setOverlayElement (Ogre::OverlayElement* ole);
    void setSpawned (void); //Marks the car as spawned
    Ogre::OverlayElement* getOverlayElement (void);

	InputState* newInput;

	RakNet::RakNetGUID getPlayerGUID();
	void setPlayerGUID(RakNet::RakNetGUID playerGUID);

private:
    const float cameraRotationConstant;
	int		hp;

    Car* mCar;
    CarSnapshot* mCarSnapshot;
    int mCarType;

    char* mNickname;
	Ogre::OverlayElement* mOLE;
	bool mSpawned;
	RakNet::RakNetGUID mPlayerGUID;
};

#endif // #ifndef PLAYER_H
