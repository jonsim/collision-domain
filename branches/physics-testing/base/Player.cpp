/**
 * @file	Player.cpp
 * @brief 	Contains the Player nodes and the related data.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"
#include "Car.h"



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
    std::string uniqueItemNo = Ogre::StringConverter::toString(physicsCore->getUniqueEntityID());

    // First set up the scene node relationships


    // lets fuck up some cars
    Car *car = new Car(sm, physicsCore->mWorld, 0);
    for (int i=1; i < 40; i++)
    {
        car = new Car(sm, physicsCore->mWorld, i);
    }

    // only attach a camera to one of them!! Imagine the carnage if there were more
    camNode = car->attachCamNode();
    camArmNode = camNode->getParentSceneNode();

    camArmNode->translate(0, 10, 0);
    camArmNode->pitch(Ogre::Degree(25));
    camNode->yaw(Ogre::Degree(180));
    camNode->translate(0, 0, -10);
}


/*void Player::createGeometry(Ogre::SceneManager *sm,
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
}*/


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
    //playerNode->setOrientation(Ogre::Quaternion(Ogre::Radian(newState.getRotation()), Ogre::Vector3::UNIT_Y));
    //mVehicle->applyEngineForce(5000., 2);
    //mVehicle->applyEngineForce(5000., 3);

}


/// @brief  Rotates the Player's wheels using some pretty bang tidy Quaternion maths. This can be removed when the physics shit is working.
/// @param  m   The direction to turn in (the InputState LeftRght value).
void Player::updateWheels (signed char m)
{
    //Ogre::Quaternion q = carFLWheelNode->getOrientation();
    //Ogre::Quaternion r = q + Ogre::Quaternion(Ogre::Radian(m * (PI / 6.0f)), Ogre::Vector3::UNIT_Y);
    //carFLWheelNode->setOrientation(r);
    //carFRWheelNode->setOrientation(r);
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