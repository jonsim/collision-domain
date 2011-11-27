/**
 * @file	Player.cpp
 * @brief 	Contains the Player nodes and the related data.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void)
{
    // PlayerState state configures constants and zeros values upon creation.
}


/// @brief   Deconstructor.
Player::~Player (void)
{
}


/// @brief  Creates and positions the 3D player object (and space for a camera).
/// @param  sm  The SceneManager to which the 3D player object is attached.
/// @param  t   The car model to load as the player object.
/// @param  s   The texture to apply to the car model.
void Player::createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s)
{
	Ogre::Entity* carEntity;

    // First set up the scene node relationships
	playerNode = sm->getRootSceneNode()->createChildSceneNode("PlayerNode");
	carNode = playerNode->createChildSceneNode("CarNode");
    camNode = playerNode->createChildSceneNode("CamNode");
    camNode->yaw(Ogre::Degree(180));
    camNode->translate(0, 200, -400);

    // Load the car mesh and attach it to the car node (this will be a large if statement)
    carEntity = sm->createEntity("CarEntity", "car_highpoly_body.mesh");
    carEntity->setCastShadows(true);
	carNode->attachObject(carEntity);

    // adjust the car's properties.
    carNode->scale(4, 4, 4);
    //carNode->yaw(Ogre::Degree(180));
}


/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void Player::attachCamera (Ogre::Camera* cam)
{
    camNode->attachObject(cam);
}


/// @brief  Updates the Player's PlayerState to the one provided.
/// @param  newState    The new state to update to.
void Player::updatePlayer (PlayerState newState)
{
	state = newState;

    //playerNode->setPosition(newState.getLocation());
    //playerNode->translate(playerNode->getOrientation() * newState.getLocation(), Ogre::Node::TS_WORLD); // getLocation returns the distance to move, not the location due to a slight hack to get the maths working quickly
    //playerNode->yaw(Ogre::Degree(newState.getRotation()), Ogre::Node::TS_WORLD);
    //playerNode->setOrientation(cos(newState.getRotation() / 2.0f), 0, 1, 0);
    //playerNode->yaw(Ogre::Radian(newState.getRotation()), Ogre::Node::TS_WORLD);
    //playerNode->translate(newState.getLocation());
    playerNode->setPosition(newState.getLocation());
    playerNode->setOrientation(Ogre::Quaternion(Ogre::Radian(newState.getRotation()), Ogre::Vector3::UNIT_Y));
}


/// @brief  Returns the Player's current state.
/// @return The Player's current state.
PlayerState Player::getPlayerState (void)
{
	return state;
}