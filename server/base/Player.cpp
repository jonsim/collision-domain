/**
 * @file	Player.cpp
 * @brief 	Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"


#define INITIAL_HEALTH 100
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
	//Set HP. More clever damage might be implemented in the future
	hp = INITIAL_HEALTH;
	roundScore = 0;
	this->setAlive(true);

	//Calculate some random to spawn
	int spawnX = rand() % 100 - 50;
	int spawnZ = rand() % 100 - 50;
    mCar->moveTo(btVector3(spawnX,0.5,spawnZ));
}


/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
///         In total this will even out to 60 calls per second :-)
/// @param  damage   0 if no damage was done to this player in the collision, else 1.
void Player::collisionTickCallback(btVector3 &hitPoint, float damage, Player *causedByPlayer) {
	if(numCollisionDataPoints < 150) {

		std::stringstream ss;
        ss << "COLLISION " << causedByPlayer->getGUID() << "\n";
        OutputDebugString(ss.str().c_str());
		numCollisionDataPoints = 0;
	}
	numCollisionDataPoints++;
	//collisionPositions[causedByPlayer->getGUID()] += hitPoint;
	//collisionDamages[causedByPlayer->getGUID()] += damage
    //OutputDebugString("Client: Player collision\n");

    // p1 and p2 might not be the only two players who collided this physics step.
    //OutputDebugString("Server: Player collision\n");
	if(GameCore::mGameplay->mGameActive && mAlive)
	{
		hp-=damage; //Apply damage to player
		GameCore::mGameplay->notifyDamage(this);

		//Force health to never drop below 0
		if(hp <= 0)
		{
			hp = 0;
			this->killPlayer(causedByPlayer);
		}
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
		mCar->steerInputTick(userInput->isLeft(), userInput->isRight(), secondsSinceLastFrame, targetPhysicsFrameRate);
    
	    // apply acceleration 4wd style
	    mCar->accelInputTick(userInput->isForward(), userInput->isBack(), userInput->isHandbrake(), secondsSinceLastFrame);
	}
    // TELEPORT TESTING
    /*if (userInput->isLeft() && userInput->isRight())
    {
        // teleport to the previously set point!
        if (mCarSnapshot != NULL) mCar->restoreSnapshot(mCarSnapshot);
    }
    
    if (userInput->isLeft() && !userInput->isRight())
    {
        // set the new teleport point
        if (mCarSnapshot != NULL) delete mCarSnapshot;
        mCarSnapshot = mCar->getCarSnapshot();
    }*/
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

    // Place an explosion at the players position and load the burnt model
    GameCore::mGraphicsCore->generateExplosion(mCar->mBodyNode->getPosition());
    mCar->loadDestroyedModel();

    // Blast the fuck out of the car (renders it completely undriveable but since this
    // should only be called on dead cars thats not such a problem).
    // Yeah so turns out this just fucks everything up and not in a good way.
    mCar->applyForce(mCar->mBodyNode, Ogre::Vector3(0, 10, 0)); 
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