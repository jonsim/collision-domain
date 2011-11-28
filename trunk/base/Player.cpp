/**
 * @file	Player.cpp
 * @brief 	Contains the Player nodes and the related data.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"



/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void) : cameraRotationConstant(0.08f)
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
    Ogre::Entity* carEntityAttachment;

    // First set up the scene node relationships
	playerNode = sm->getRootSceneNode()->createChildSceneNode("PlayerNode");
    camArmNode = playerNode->createChildSceneNode("CamArmNode");
	camNode = camArmNode->createChildSceneNode("CamNode");
	carNode = playerNode->createChildSceneNode("CarNode");
	carLDoorNode = carNode->createChildSceneNode("CarLDoorNode");
	carRDoorNode = carNode->createChildSceneNode("CarRDoorNode");
	carFBumperNode = carNode->createChildSceneNode("CarFBumperNode");
	carRBumperNode = carNode->createChildSceneNode("CarRBumperNode");
	carFLWheelNode = carNode->createChildSceneNode("CarFLWheelNode");
	carFRWheelNode = carNode->createChildSceneNode("CarFRWheelNode");
	carRLWheelNode = carNode->createChildSceneNode("CarRLWheelNode");
	carRRWheelNode = carNode->createChildSceneNode("CarRRWheelNode");

    // sort out the camera's shit
    camArmNode->translate(0, 100, 0);
    camArmNode->pitch(Ogre::Degree(25));
    camNode->yaw(Ogre::Degree(180));
    camNode->translate(0, 0, -500);

    // Load the car mesh and attach it to the car node (this will be a large if statement for all models/meshes)
    carEntity = sm->createEntity("CarEntity", "car2_body.mesh");
    carEntity->setMaterialName("car2_body");
    carEntity->setCastShadows(true);
	carNode->attachObject(carEntity);
    //carNode->translate(0, 0, 100);
    
    // load the left door baby
    carEntityAttachment = sm->createEntity("CarEntity_LDoor", "car2_door.mesh");
    carEntityAttachment->setMaterialName("car2_door");
    carEntityAttachment->setCastShadows(true);
    carLDoorNode->attachObject(carEntityAttachment);
    carLDoorNode->translate(43, 20, 22);
    
    // lets get a tasty right door
    carEntityAttachment = sm->createEntity("CarEntity_RDoor", "car2_door.mesh");
    carEntityAttachment->setMaterialName("car2_door");
    carEntityAttachment->setCastShadows(true);
    carRDoorNode->attachObject(carEntityAttachment);
    carRDoorNode->scale(-1, 1, 1);
    carRDoorNode->translate(-46, 20, 22);

    // and now a sweet sweet front bumper
    carEntityAttachment = sm->createEntity("CarEntity_FBumper", "car2_Fbumper.mesh");
    carEntityAttachment->setMaterialName("car2_Fbumper");
    carEntityAttachment->setCastShadows(true);
    carFBumperNode->attachObject(carEntityAttachment);
    carFBumperNode->translate(0, 20, 140);

    // and now a regular rear bumper
    carEntityAttachment = sm->createEntity("CarEntity_RBumper", "car2_Rbumper.mesh");
    carEntityAttachment->setMaterialName("car2_Rbumper");
    carEntityAttachment->setCastShadows(true);
    carRBumperNode->attachObject(carEntityAttachment);
    carRBumperNode->translate(0, 20, -135);

    // tidy front left wheel
    carEntityAttachment = sm->createEntity("CarEntity_FLWheel", "car2_wheel.mesh");
    carEntityAttachment->setMaterialName("car2_wheel");
    carEntityAttachment->setCastShadows(true);
    carFLWheelNode->attachObject(carEntityAttachment);
    carFLWheelNode->translate(45, 18, 95);

    // delightful front right wheel
    carEntityAttachment = sm->createEntity("CarEntity_FRWheel", "car2_wheel.mesh");
    carEntityAttachment->setMaterialName("car2_wheel");
    carEntityAttachment->setCastShadows(true);
    carFRWheelNode->attachObject(carEntityAttachment);
    carFRWheelNode->translate(-45, 18, 95);
    carFRWheelNode->scale(-1, 1, 1);

    // and now an arousing rear left wheel
    carEntityAttachment = sm->createEntity("CarEntity_RLWheel", "car2_wheel.mesh");
    carEntityAttachment->setMaterialName("car2_wheel");
    carEntityAttachment->setCastShadows(true);
    carRLWheelNode->attachObject(carEntityAttachment);
    carRLWheelNode->translate(45, 18, -72);

    // and finally a rear right wheel to seal the deal. beaut.
    carEntityAttachment = sm->createEntity("CarEntity_RRWheel", "car2_wheel.mesh");
    carEntityAttachment->setMaterialName("car2_wheel");
    carEntityAttachment->setCastShadows(true);
    carRRWheelNode->attachObject(carEntityAttachment);
    carRRWheelNode->translate(-45, 18, -72);
    carRRWheelNode->scale(-1, 1, 1);
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

    playerNode->setPosition(newState.getLocation());
    playerNode->setOrientation(Ogre::Quaternion(Ogre::Radian(newState.getRotation()), Ogre::Vector3::UNIT_Y));
}


/// @brief  Rotates the Player's wheels using some pretty bang tidy Quaternion maths. This can be removed when the physics shit is working.
/// @param  m   The direction to turn in (the InputState LeftRght value).
void Player::updateWheels (signed char m)
{
    Ogre::Quaternion q = carFLWheelNode->getOrientation();
    Ogre::Quaternion r = q + Ogre::Quaternion(Ogre::Radian(m * (PI / 6.0f)), Ogre::Vector3::UNIT_Y);
    carFLWheelNode->setOrientation(r);
    carFRWheelNode->setOrientation(r);
}


/// @brief  Updates the camera's rotation based on the values given.
/// @param  XRotation   The amount to rotate the camera by in the X direction (relative to its current rotation).
/// @param  YRotation   The amount to rotate the camera by in the Y direction (relative to its current rotation).
void Player::updateCamera (int XRotation, int YRotation)
{
    camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
    camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * YRotation), Ogre::Node::TS_LOCAL);
}


/// @brief  Returns the Player's current state.
/// @return The Player's current state.
PlayerState Player::getPlayerState (void)
{
	return state;
}