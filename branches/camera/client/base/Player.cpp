/**
 * @file	Player.cpp
 * @brief 	Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>

/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void) : cameraRotationConstant(0.08f), mAlive(true), mIsVIP(false), 
                        mTeam(0), mCarSnapshot(NULL), mSnapshots(NULL), mCar(NULL)
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
/// @param  physicsCore   The class containing the physics world.
void Player::createPlayer (CarType carType, CarSkin skin)
{
    mCarType = carType;

    switch (carType)
    {
    case CAR_BANGER:
        mCar = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID(), skin);
        break;
    case CAR_SMALL:
        mCar = (Car*) new SmallCar(GameCore::mPhysicsCore->getUniqueEntityID(), skin);
        break;
    case CAR_TRUCK:
        mCar = (Car*) new TruckCar(GameCore::mPhysicsCore->getUniqueEntityID(), skin);
        break;
    default:
        mCar = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID(), skin);
        break;
    }

    mCar->attachCollisionTickCallback(this);

    mCar->moveTo(btVector3(0,0.5,0));
}


/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
/// @param  damage   Currently hardcoded to 1
void Player::collisionTickCallback(int damage)
{
    OutputDebugString("Client: Player collision\n");
}


/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void Player::attachCamera (Ogre::Camera* cam)
{
    // only attach a camera to one of them!! Imagine the carnage if there were more
    camNode = mCar->attachCamNode();
    camArmNode = camNode->getParentSceneNode();
    camNode->translate(0, 0, -20); // zoom in!! (50 is a fair way behind the car, 75 is in the car)

    // Create game camera
    mCamera = new GameCamera( cam );
    // Set it to chase mode
    mCamera->setCamType( CAM_CHASE );
    // Set how much the camera 'snaps' to locations
    // This gets multiplied by time since last frame
    // For cinematic style camera 0.2 works quite well
    mCamera->setTension( 2.8f );
    // Chase the car body
    mCamera->setTarget( getCar()->mBodyNode );
    // Positional offset - behind and above the vehicle
    mCamera->setOffset( btVector3( 0.f, 5.f, -10.f ) );
    // Focus offset - slightly in front of car's local origin
    mCamera->setLookOffset( btVector3( 0, 0, 3.0f ) );
    // Put the camera up in the air
    mCamera->setTransform( btVector3( 0, 20, 0 ) );

}


/// @brief  Applies the player controls to the car so it will move on next stepSimulation.
/// @param  userInput               The latest user keypresses.
/// @param  secondsSinceLastFrame   The time in seconds since the last frame, for framerate independence.
/// @param  targetPhysicsFrameRate  The target framerate to normalise acceleration to.
void Player::processControlsFrameEvent(
        InputState *userInput,
        Ogre::Real secondsSinceLastFrame,
        float targetPhysicsFrameRate)
{
	//Only take input if the player is alive
	if(this->mAlive)
	{
		// process steering and apply acceleration
		mCar->steerInputTick(userInput->isLeft(), userInput->isRight(), secondsSinceLastFrame, targetPhysicsFrameRate);
		mCar->accelInputTick(userInput->isForward(), userInput->isBack(), userInput->isHandbrake(), secondsSinceLastFrame);
	}
}


/// @brief  Updates the camera's rotation based on the values given.
/// @param  XRotation   The amount to rotate the camera by in the X direction (relative to its current rotation).
/// @param  YRotation   The amount to rotate the camera by in the Y direction (relative to its current rotation).
void Player::updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth, float time)
{
    //Ogre::SceneNode *camNode = mCar->attachCamNode();
    //Ogre::SceneNode *camArmNode = camNode->getParentSceneNode();
    
    camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
	camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * -YRotation), Ogre::Node::TS_LOCAL);

	Ogre::Vector3 camPosition = camNode->getPosition();
	ZDepth = -ZDepth;
	if ((ZDepth < 0 && camPosition.z > -40) || (ZDepth > 0 && camPosition.z < 90))
		camNode->translate(0, 0, ZDepth * 0.02f);

    mCamera->update(time);

	//Update the camera
	//
	/*
	std::stringstream ssTmp;
	ssTmp << "Position of CamNode: " << camArmNode->getPosition() << "\n";
	OutputDebugString(ssTmp.str().c_str());
	*/
}

/// @brief Returns the camera current yawing around the player.
/// @return The yawing, in degrees, around the player. 0 is directly in front of the player, +/-180 is behind.
float Player::getCameraYaw ()
{
	return camArmNode->getOrientation().getYaw().valueDegrees();
}


/// @brief  Supplies the Car object which contains player position and methods on that. 
/// @return The Car object which allows forcing a player to a given CarSnapshot or getting a CarSnapshot.
Car* Player::getCar()
{
    return mCar;
}

void Player::applyHealthBonus()
{

}

int Player::getHP()
{
	return hp;
}

void Player::killPlayer()
{
	this->mAlive = false;
    // Place an explosion at the players position and load the burnt model
    GameCore::mGraphicsCore->generateExplosion(mCar->mBodyNode->getPosition());
    mCar->loadDestroyedModel();
}

void Player::setAlive(bool pAlive)
{
	mAlive = pAlive;
}

bool Player::getAlive()
{
	return mAlive;
}