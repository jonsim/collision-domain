/**
 * @file	Player.h
 * @brief 	Contains the Player nodes and the related data.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "PlayerState.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
/// CarType represents the type of car used as the player object (i.e. the model).
enum CarType {SMALL, MEDIUM, LARGE};

/// CarSkin represents the texture applied to the player object.
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

class Player //: public PhysicsEntity
{
 
public:
    Player (void);
    ~Player (void);
    void createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s);
    void attachCamera (Ogre::Camera* cam);
	void updatePlayer (PlayerState newState);
	PlayerState capturePlayer (void);
    
private:
	PlayerState state;
    Ogre::SceneNode *playerNode;	///< The complete player node - this is what should be moved when the car moves.
    Ogre::SceneNode *carNode;		///< The node onto which the car object and its components attach.
    Ogre::SceneNode *camNode;		///< The node onto which a camera can be attached to observe the car.
	
    int   playerTopSpeed;             	///< The maximum speed of the player (units / sec).
    int   playerAccelerationConstant; 	///< The amount that the player linearly accelerates by when moving forward (units / sec^2).
	int   playerTurningConstant;      	///< The amount that the player turns when cornering (degrees / sec).
    float playerFrictionConstant;   	///< The amount of energy a player loses per second to friction (percentage).
};

#endif // #ifndef PLAYER_H
