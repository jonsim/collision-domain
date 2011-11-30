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
void Player::createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s, PhysicsCore *physicsCore)
{
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
    createGeometry(sm, "CarEntity", "car2_body.mesh", "car2_body", carNode);
    //carNode->translate(0, 0, 100);
    
    // load the left door baby
    createGeometry(sm, "CarEntity_LDoor", "car2_door.mesh", "car2_door", carLDoorNode);
    carLDoorNode->translate(43, 20, 22);
    
    // lets get a tasty right door
    createGeometry(sm, "CarEntity_RDoor", "car2_door.mesh", "car2_door", carRDoorNode);
    carRDoorNode->scale(-1, 1, 1);
    carRDoorNode->translate(-46, 20, 22);

    // and now a sweet sweet front bumper
    createGeometry(sm, "CarEntity_FBumper", "car2_Fbumper.mesh", "car2_Fbumper", carFBumperNode);
    carFBumperNode->translate(0, 20, 140);

    // and now a regular rear bumper
    createGeometry(sm, "CarEntity_RBumper", "car2_Rbumper.mesh", "car2_Rbumper", carRBumperNode);
    carRBumperNode->translate(0, 20, -135);

    // tidy front left wheel
    createGeometry(sm, "CarEntity_FLWheel", "car2_wheel.mesh", "car2_wheel", carFLWheelNode);
//    carFLWheelNode->translate(45, 18, 95);

    // delightful front right wheel
    createGeometry(sm, "CarEntity_FRWheel", "car2_wheel.mesh", "car2_wheel", carFRWheelNode);
//    carFRWheelNode->translate(-45, 18, 95);
    carFRWheelNode->scale(-1, 1, 1);

    // and now an arousing rear left wheel
    createGeometry(sm, "CarEntity_RLWheel", "car2_wheel.mesh", "car2_wheel", carRLWheelNode);
//    carRLWheelNode->translate(45, 18, -72);

    // and finally a rear right wheel to seal the deal. beaut.
    createGeometry(sm, "CarEntity_RRWheel", "car2_wheel.mesh", "car2_wheel", carRRWheelNode);
//    carRRWheelNode->translate(-45, 18, -72);
    carRRWheelNode->scale(-1, 1, 1);


    const Ogre::Vector3 carPosition(15, 30,-25);
    const Ogre::Vector3 chassisShift(0, 1.0, 0);
    physicsCore->newCar(carPosition, chassisShift, playerNode, carFLWheelNode, carFRWheelNode, carRLWheelNode, carRRWheelNode);
}


void Player::createGeometry(Ogre::SceneManager *sm,
                            const std::string &entityName,
                            const std::string &meshName,
                            const std::string &materialName,
                            Ogre::SceneNode *toAttachTo)
{
    Ogre::Entity* entity;

    entity = sm->createEntity(entityName, meshName);
    entity->setMaterialName(materialName);

    entity->setQueryFlags(GEOMETRY_QUERY_MASK); // lets raytracing hit this object (for physics)
#if (OGRE_VERSION < ((1 << 16) | (5 << 8) | 0))
    entity->setNormaliseNormals(true);
#endif // only applicable before shoggoth (1.5.0)

    entity->setCastShadows(true);
    toAttachTo->attachObject(entity);
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