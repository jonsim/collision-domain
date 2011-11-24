/*------------------------------------------------------------------------------
  File:     player.cpp
  Purpose:  
 ------------------------------------------------------------------------------*/

/******************** DEFINITIONS ********************/

/******************** INCLUDES ********************/
#include "../graphics/includes/stdafx.h"
#include "../base/includes/playerSnapshot.h"
#include "includes/player.h"
#include <iostream>



/******************** METHOD DEFINITIONS ********************/

/*------------------------------------------------------------------------------
  Method:       player::player(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Constructor.
 ------------------------------------------------------------------------------*/
player::player (void)
{
playerTurningConstant = 0.6;       // The amount that the player turns when cornering (?? / sec)
playerAccelerationConstant = 800;  // The amount that the player linearly accelerates by when moving forward (units / sec^2)
playerTopSpeed = 1000;             // The maximum speed of the player (units / sec)
playerFrictionConstant = 0.4;      // The amount of energy a player loses per second to friction (percentage)

	playerSpeed = 0;
	playerAcceleration = 0;
    playerRotation = 0;
}


/*------------------------------------------------------------------------------
  Method:       player::~player(void)
  Parameters:   N/A
  Outputs:      N/A
  Purpose:      Deconstructor.
 ------------------------------------------------------------------------------*/
player::~player (void)
{
}


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


void player::attachCamera (Ogre::Camera* cam)
{
    camNode->attachObject(cam);
}


void player::calculateState (playerSnapshot snapshot, Ogre::Real timeSinceLastFrame)
{
    playerSpeed += ((int) snapshot.revealForward()) * playerAccelerationConstant * timeSinceLastFrame;
    playerSpeed -= playerSpeed * playerFrictionConstant * timeSinceLastFrame;
    if (playerSpeed > playerTopSpeed)
        playerSpeed = playerTopSpeed;
    
    playerRotation = ((int) snapshot.revealTurn()) * playerTurningConstant;

    std::cerr << "Speed = " << playerSpeed << std::endl;

    playerNode->translate(Ogre::Vector3(0, 0, playerSpeed * timeSinceLastFrame), Ogre::Node::TS_LOCAL);
    playerNode->yaw(Ogre::Degree(playerRotation), Ogre::Node::TS_WORLD);
}