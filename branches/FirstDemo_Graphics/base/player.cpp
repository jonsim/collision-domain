/**
 * @file	Player.cpp
 * @brief 	Contains the Player nodes and the related data.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
player::player (void)
{
    playerTurningConstant = 0.6;
    playerAccelerationConstant = 800;
    playerTopSpeed = 1000;
    playerFrictionConstant = 0.4;
}


/// @brief   Deconstructor.
player::~player (void)
{
}


/// @brief  Creates and positions the 3D player object (and space for a camera).
/// @param  sm  The SceneManager to which the 3D player object is attached.
/// @param  t   The car model to load as the player object.
/// @param  s   The texture to apply to the car model.
void player::createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s)
{
	Ogre::Entity* carEntity;

    // First set up the scene node relationships
	playerNode = sm->getRootSceneNode()->createChildSceneNode("PlayerNode");
	carNode = playerNode->createChildSceneNode("CarNode");
    camNode = playerNode->createChildSceneNode("CamNode");
    camNode->translate(0, 200, 400);

    // Load the car mesh and attach it to the car node (this will be a large if statement)
    carEntity = sm->createEntity("CarEntity", "car_highpoly_body.mesh");
    carEntity->setCastShadows(true);
	carNode->attachObject(carEntity);

    // adjust the car's properties.
    carNode->scale(4, 4, 4);
    carNode->yaw(Ogre::Degree(180));
}


/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void player::attachCamera (Ogre::Camera* cam)
{
    camNode->attachObject(cam);
}


/// @brief  Updates the Player's PlayerState to the one provided.
/// @param  newState    The new state to update to.
void player::updatePlayer (PlayerState newState)
{
	state = newState;
}


/// @brief  Returns the Player's current state.
/// @return The Player's current state.
PlayerState player::capturePlayer (void)
{
	return state;
}


/*void player::calculateState (playerSnapshot snapshot, Ogre::Real timeSinceLastFrame)
{
    playerSpeed += ((int) snapshot.revealForward()) * playerAccelerationConstant * timeSinceLastFrame;
    playerSpeed -= playerSpeed * playerFrictionConstant * timeSinceLastFrame;
    if (playerSpeed > playerTopSpeed)
        playerSpeed = playerTopSpeed;
    
    playerRotation = ((int) snapshot.revealTurn()) * playerTurningConstant;

    std::cerr << "Speed = " << playerSpeed << std::endl;

    playerNode->translate(Ogre::Vector3(0, 0, playerSpeed * timeSinceLastFrame), Ogre::Node::TS_LOCAL);
    playerNode->yaw(Ogre::Degree(playerRotation), Ogre::Node::TS_WORLD);
}*/