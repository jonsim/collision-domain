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
    std::string uniqueItemNo = Ogre::StringConverter::toString(physicsCore->getUniqueEntityID());

    // First set up the scene node relationships

    mCar = (Car*) new SimpleCoupeCar(sm, physicsCore->mWorld, 0);
    // lets fuck up some cars
    
    new BulletBuggyCar(sm, physicsCore->mWorld, 1);

    for (int i=1; i < 10; i++)
    {
       // new Car(sm, physicsCore->mWorld, i);
    }

}


/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void Player::attachCamera (Ogre::Camera* cam)
{
    // only attach a camera to one of them!! Imagine the carnage if there were more
    Ogre::SceneNode *camNode = mCar->attachCamNode();
    Ogre::SceneNode *camArmNode = camNode->getParentSceneNode();

    camArmNode->translate(0, 0.5, 0); // place camera y above car node
    camArmNode->pitch(Ogre::Degree(25));
    camNode->yaw(Ogre::Degree(180));
    camNode->translate(0, 0, 62); // zoom in!! (50 is a fair way behind the car, 75 is in the car)

    camNode->attachObject(cam);
}


/// @brief  Updates the Player's PlayerState to the one provided.
/// @param  newState    The new state to update to.
void Player::processControlsTick(Input *userInput)
{
    // apply csp
    // create new PlayerState newState from generated controls
    
    //state = newState; - store the states for later
    
    // process steering
    mCar->steerInputTick(
        userInput->mKeyboard->isKeyDown(OIS::KC_A),
        userInput->mKeyboard->isKeyDown(OIS::KC_D));
    
    // apply acceleration 4wd style
    mCar->accelInputTick(
        userInput->mKeyboard->isKeyDown(OIS::KC_W),
        userInput->mKeyboard->isKeyDown(OIS::KC_S));

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
    Ogre::SceneNode *camArmNode = mCar->attachCamNode()->getParentSceneNode();
    camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
    camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * YRotation), Ogre::Node::TS_LOCAL);
}


/// @brief  Returns the Player's current state.
/// @return The Player's current state.
PlayerState Player::getPlayerState (void)
{
    return state;
}