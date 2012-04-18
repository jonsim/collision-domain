/**
 * @file	Player.cpp
 * @brief 	Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


#define INITIAL_HEALTH 600
#define MAX_DAMAGE 400 // used cap damage for individual crashes so that deformations are more managable
#define BIG_CRASH_THRESHOLD 80
/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void) : cameraRotationConstant(0.08f), mSpawned(false), mAlive(false),
                        mIsVIP(false), mTeam(0), mCarSnapshot(NULL), newInput(NULL), mCar(NULL)
{
    // PlayerState state configures constants and zeros values upon creation.
}


/// @brief   Deconstructor.
Player::~Player (void)
{
	if( mCar )
		delete( mCar );
}


/// @brief  Creates and positions the 3D player object (and space for a camera).
/// @param  sm  The SceneManager to which the 3D player object is attached.
/// @param  t   The car model to load as the player object.
/// @param  s   The texture to apply to the car model.
/// @param  physicsCore   The class containing the physics world.
void Player::createPlayer (CarType carType, TeamID tid)
{
    mCarType = carType;

    switch (carType)
    {
    case CAR_BANGER:
        mCar = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid);
        break;
    case CAR_SMALL:
        mCar = (Car*) new SmallCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid);
        break;
    case CAR_TRUCK:
        mCar = (Car*) new TruckCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid);
        break;
    default:
        throw Ogre::Exception::ERR_INVALIDPARAMS;
        break;
    }
	
    mCar->attachCollisionTickCallback(this);
	//Set HP. More clever damage might be implemented in the future
	hp = INITIAL_HEALTH;
	roundScore = 0;
	this->setAlive(true);

	//Calculate some random to spawn
	int spawnX = rand() % 100 - 50;
	int spawnZ = rand() % 100 - 50;
    mCar->moveTo(btVector3(spawnX,0.5,spawnZ));
	mSpawned = true;

    initialHP = INITIAL_HEALTH;
	backRightDamageShare = 0.3f;
	backLeftDamageShare = 0.3f;
	frontLeftDamageShare = 0.2f;
	frontRightDamageShare = 0.2f;
}


/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
///         In total this will even out to 60 calls per second :-)
/// @param  damage   0 if no damage was done to this player in the collision, else 1.
void Player::collisionTickCallback(btVector3 &hitPoint, float depth, Player *causedByPlayer) {
		// convert the hitPoint to an ogre vector in our local space, to pass to deformer
	Ogre::Vector3 adjust = this->getCar()->mBodyNode->convertWorldToLocalPosition((Ogre::Vector3)hitPoint);
	// calculate the unsigned yaw rotation to the adjusted hitpoint, gives us a crude but usable mapping for the damage HUD
	Ogre::Real or1 = this->getCar()->mBodyNode->getPosition().getRotationTo(adjust).getYaw().valueDegrees()+180;
	
	if(adjust.x == 0.f && adjust.y == 0.f && adjust.z == 0.f) {
		OutputDebugString("ZERO collision Point\n");
	}

	// combine speeds of both cars, gives approximation of total force in collision
	float p1Speed = this->getCar()->getCarMph();
	float p2Speed = causedByPlayer->getCar()->getCarMph();
	float combinedSpeed = p1Speed + p2Speed;

	// calculate ratio of damage to each player from the combined speed
	// these will then be multiplied by the totalDamage to get amount of damage to each car
	float damageShareTo1 = p2Speed / combinedSpeed;
	float damageShareTo2 = p1Speed / combinedSpeed;

	float totalDamage = abs(depth * 1000);
	totalDamage = totalDamage > MAX_DAMAGE ? MAX_DAMAGE : totalDamage; 
	float damageToThis = totalDamage * damageShareTo1;

	std::stringstream ss;
	//ss << "totDamage " << totalDamage << "\n";
	//OutputDebugString(ss.str().c_str());

	// differentiate between differnt collision types
	if(totalDamage < BIG_CRASH_THRESHOLD && (p1Speed > 40 || p2Speed > 40)) {
		//OutputDebugString("Gleam\n");
	} else if(totalDamage < BIG_CRASH_THRESHOLD && (p1Speed < 40 && p2Speed < 40)) {
		//OutputDebugString("Bump\n");
	} else if(totalDamage >= BIG_CRASH_THRESHOLD) {
		//OutputDebugString("Bang\n");
		// Uncomment to have deformations on server!
		//GameCore::mGraphicsCore->meshDeformer->collisonDeform(this->getCar()->mBodyNode, (Ogre::Vector3)hitPoint, damageToThis);
	}
	// map damage to HUD
	//std::stringstream ss;
	if(or1 >= 0 && or1 < 90) {
		//ss << "front right ";
		frontRightDamage += damageToThis;
	} else if(or1 >= 90 && or1 < 180) {
		//ss << "front left ";
		frontLeftDamage += damageToThis;
	} else if(or1 >= 180 && or1 < 270) {
		//ss << "back left ";
		backLeftDamage += damageToThis;
	} else if(or1 >= 270 && or1 < 360) {
		//ss << "back right ";
		backRightDamage += damageToThis;
	}
	
	//OutputDebugString("Sever collision\n");

	if((GameCore::mGameplay->mGameActive && mAlive) || 2+2==4) {
		hp = recalculateDamage();
		std::stringstream ss;
		ss << "hp = " << hp << "\n";
		OutputDebugString(ss.str().c_str());
		GameCore::mGameplay->notifyDamage(this);
		//Force health to never drop below 0
		if(hp <= 0) {
			hp = 0;
			this->killPlayer(causedByPlayer);
		}
	}
}

float Player::recalculateDamage(void) {
	return initialHP - (
		backRightDamage  * backRightDamageShare +
		backLeftDamage   * backLeftDamageShare +
		frontRightDamage * frontRightDamageShare +
		frontLeftDamage  * frontLeftDamageShare 
    );
}

void Player::cameraLookLeft(void) {
	OutputDebugString("look left\n");
}

void Player::cameraLookRight(void) {
	OutputDebugString("look right\n");
}

void Player::cameraLookBack(void) {
	OutputDebugString("look back\n");
}

void Player::revertCamera(void) {
	OutputDebugString("revert\n");
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


/// @brief  Applies the player controls to the car so it will move on next stepSimulation.
/// @param  userInput               The latest user keypresses.
/// @param  secondsSinceLastFrame   The time in seconds since the last frame, for framerate independence.
/// @param  targetPhysicsFrameRate  The target framerate to normalise acceleration to.
void Player::processControlsFrameEvent(
        InputState *userInput,
        Ogre::Real secondsSinceLastFrame,
        float targetPhysicsFrameRate)
{
	if(this->getAlive())
	{
		// process steering
		mCar->steerInputTick(userInput->isLeft(), userInput->isRight(), secondsSinceLastFrame);
    
	    // apply acceleration 4wd style
	    mCar->accelInputTick(userInput->isForward(), userInput->isBack(), userInput->isHandbrake(), secondsSinceLastFrame);
	}
    else
    {
        mCar->steerInputTick(false, false, secondsSinceLastFrame);
        mCar->accelInputTick(false, false, false, secondsSinceLastFrame);
    }
}


/// @brief  Updates the camera's rotation based on the values given.
/// @param  XRotation   The amount to rotate the camera by in the X direction (relative to its current rotation).
/// @param  YRotation   The amount to rotate the camera by in the Y direction (relative to its current rotation).
void Player::updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth)
{
    //Ogre::SceneNode *camArmNode = mCar->attachCamNode()->getParentSceneNode();

    camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
    camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * YRotation), Ogre::Node::TS_LOCAL);

    
    camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
	camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * -YRotation), Ogre::Node::TS_LOCAL);

	Ogre::Vector3 camPosition = camNode->getPosition();
	ZDepth = -ZDepth;
	if ((ZDepth < 0 && camPosition.z > -40) || (ZDepth > 0 && camPosition.z < 90))
		camNode->translate(0, 0, ZDepth * 0.02f);

    mCarCam->updatePosition(XRotation, YRotation);
}


/// @brief  Supplies the Car object which contains player position and methods on that. 
/// @return The Car object which allows forcing a player to a given CarSnapshot or getting a CarSnapshot.
Car* Player::getCar()
{
    return mCar;
}


int Player::getHP()
{
	return hp;
}

void Player::resetHP()
{
	this->hp = INITIAL_HEALTH;
	this->mAlive = true;
}

void Player::applyHealthBonus()
{

}

void Player::setOverlayElement(Ogre::OverlayElement* ole)
{
        mOLE = ole;
}

Ogre::OverlayElement* Player::getOverlayElement()
{
        return mOLE;
}

void Player::setSpawned()
{
        mSpawned = true;
}

RakNet::RakNetGUID Player::getPlayerGUID()
{
	return mPlayerGUID;
}

void Player::setPlayerGUID(RakNet::RakNetGUID playerGUID)
{
	mPlayerGUID = playerGUID;
}

void Player::setAlive(bool pAlive)
{
	mAlive = pAlive;
}

bool Player::getAlive()
{
	return mAlive;
}

void Player::killPlayer()
{
	mAlive = false;

	GameCore::mNetworkCore->sendPlayerDeath(this);

    mCar->applyForce(mCar->mBodyNode, Ogre::Vector3(0, 500.0f, 0)); 
}

void Player::setGUID(RakNet::RakNetGUID playerGUID) {
	stringGUID = playerGUID.ToString();
}

std::string Player::getGUID(void) {
	return stringGUID;
}

void Player::killPlayer(Player* causedBy)
{
	this->killPlayer();
	GameCore::mGameplay->markDeath(this,causedBy);

	if(this->getVIP())
	{
		causedBy->addToScore(5);
	}
	else
	{
		causedBy->addToScore(1);
	}
}

void Player::addToScore(int amount)
{
	roundScore += amount;
}

int Player::getRoundScore()
{
	return this->roundScore;
}

void Player::addToGameScore(int amount)
{
	this->gameScore += amount;
}