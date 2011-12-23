/**
 * @file	Player.h
 * @brief 	Contains the Player nodes and the related data.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "PlayerState.h"
#include "InputState.h"
#include "PhysicsCore.h"
#include "Car.h"
#include "cars/BulletBuggyCar.h"
#include "cars/SimpleCoupeCar.h"
#include "CarSnapshot.h"
#include "Input.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/// CarType represents the type of car used as the player object (i.e. the model).
enum CarType {SMALL, MEDIUM, LARGE};

/// CarSkin represents the texture applied to the player object.
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class Player //: public PhysicsEntity
{
 
public:
    Player (void);
    ~Player (void);
    void createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s, PhysicsCore *physicsCore);
    void attachCamera (Ogre::Camera* cam);
    void updatePlayer (PlayerState newState);
    void processControlsTick(Input *userInput);
    void updateWheels (signed char m);
    void updateCamera (int XRotation, int YRotation);
    PlayerState getPlayerState (void);
    
private:
    PlayerState state;              ///< The player's current state.
    const float cameraRotationConstant;

    Car *mCar;
    CarSnapshot *mCarSnapshot;
};

#endif // #ifndef PLAYER_H
