/**
 * @file	Player.cpp
 * @brief 	Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>

#define INITIAL_HEALTH 1200
#define NEWCAM 1
/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void) : cameraRotationConstant(0.08f),
                        mAlive(true),
                        mIsVIP(false), 
                        mTeam(NO_TEAM),
                        mCarSnapshot(NULL),
                        mSnapshots(NULL),
                        mCar(NULL)
{
    // PlayerState state configures constants and zeros values upon creation.
	//processingCollision = false;
	numCollisionDataPoints = 0;
	this->mOLE = NULL;
	//averageCollisionPoint.setZero();

	// Damage Level Thresholds: how many calls to collisionTickCallback have been seen
	//lowDamageThreshold = 25;
	//mediumDamageThreshold = 55;
	//highDamageThreshold = 80;
	//btVector3::setZero(averageCollisionPoint);
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
    
    bool isLocalPlayer = this == GameCore::mPlayerPool->getLocalPlayer();
    if (isLocalPlayer)
    {
        mCar->louderLocalSounds();
    }

	roundScore = 0;
    mCar->attachCollisionTickCallback(this);
    
    mCar->moveTo(btVector3(0,0.5,0));
}


/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
/// @param  hitPoint		Location of the collision point on the collision mesh - in world coordinates
/// @param  speed			The speed of the impact in the direction of the normal to the collision point
/// @param  causedByPlayer	Pointer to the other player in the collision.
void Player::collisionTickCallback(btVector3 &hitPoint, float depth, Player *causedByPlayer) {
	/*float hisSpeed = causedByPlayer->getCar()->getCarMph();
	float mySpeed = getCar()->getCarMph();
	if(depth < 
	if(speed >= lowDamageSpeed && speed < mediumDamageSpeed) {
		lowDamageCallBack(causedByPlayer->getGUID());
	} else if(speed >= mediumDamageSpeed && speed < highDamageSpeed) {
		midDamageCallBack(causedByPlayer->getGUID());
	} else if(speed >= highDamageSpeed) {
		highDamageCallBack(causedByPlayer->getGUID());
	}*/
	GameCore::mClientGraphics->mMeshDeformer->collisonDeform(this->getCar()->mBodyNode, (Ogre::Vector3)hitPoint);
}


/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void Player::attachCamera (Ogre::Camera* cam)
{
    // only attach a camera to one of them!! Imagine the carnage if there were more
    camNode = mCar->attachCamNode();
    camArmNode = camNode->getParentSceneNode();
    camNode->translate(0, 0, -20); // zoom in!! (50 is a fair way behind the car, 75 is in the car)

#if NEWCAM
    // Create game camera
    mCamera = new GameCamera( cam );
    // Set it to chase mode
    mCamera->setCamType( CAM_CHASE );
    mCamera->setCollidable( true );
    // Set how much the camera 'snaps' to locations
    // This gets multiplied by time since last frame
    // For cinematic style camera 0.2 works quite well
    mCamera->setTension( 2.8f );
    // Positional offset - behind and above the vehicle
    mCamera->setOffset( btVector3( 0.f, 5.f, -10.f ) );
    // Focus offset - slightly in front of car's local origin
    mCamera->setLookOffset( btVector3( 0, 0, 3.0f ) );
    // Put the camera up in the air
    mCamera->setTransform( btVector3( 0, 20, 0 ) );

    GameCore::mPlayerPool->setSpectating( GameCore::mPlayerPool->getLocalPlayerID() );
#else
    mCarCam = new CarCam(mCar,cam, camNode, mCar->mBodyNode);
#endif
}


/// @brief  Applies the player controls to the car so it will move on next stepSimulation.
/// @param  userInput               The latest user keypresses.
/// @param  secondsSinceLastFrame   The time in seconds since the last frame, for framerate independence.
void Player::processControlsFrameEvent(
        InputState *userInput,
        Ogre::Real secondsSinceLastFrame)
{
	//Only take input if the player is alive
	if(this->mAlive)
	{
		// process steering and apply acceleration
		mCar->steerInputTick(userInput->isLeft(), userInput->isRight(), secondsSinceLastFrame);
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

#if NEWCAM
    if( mCamera->getCamType() == CAM_FIXED )
        mCamera->update( Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Degree(cameraRotationConstant * 0.5f * -YRotation) );
    else
        mCamera->update(time);
#endif
   
	//Update the camera
	//
	/*
	std::stringstream ssTmp;
	ssTmp << "Position of CamNode: " << camArmNode->getPosition() << "\n";
	OutputDebugString(ssTmp.str().c_str());
	*/
}

/// @brief  Updates graphics for the local player (with effects that should only be applied from that, for example
///         radial blur, screen cracks etc.
void Player::updateLocalGraphics (void)
{
	// Update radial blur (from vehicle speed).
	float speedmph = mCar->getCarMph();
	float blurAmount = 0;

	if (speedmph > 40.0f)
	{
		// calculate blurring as a function of speed, then scale it back depending on where you
		// are looking at the car from (effect strongest from behind and infront (3 maxima at 
		// +/-180 and 0, hence the double abs() reduction)).
		blurAmount = (speedmph - 40) / 28;
		blurAmount *= abs(abs(GameCore::mPlayerPool->getLocalPlayer()->getCameraYaw()) - 90) / 90;
	}
    GameCore::mClientGraphics->setRadialBlur(GameCore::mClientGraphics->mCamera->getViewport(), blurAmount);
}

/// @brief  Updates graphics for all players (called individually for each player in player pool), contains graphical
///         effects all players will have.
void Player::updateGlobalGraphics (bool isForward, Ogre::Real secondsSinceLastFrame)
{
    mCar->updateParticleSystems(isForward, secondsSinceLastFrame);
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


void Player::killPlayer()
{
	if(!mCar)
		return;

	this->mAlive = false;
    // Place an explosion at the players position and load the burnt model
    GameCore::mClientGraphics->generateExplosion(mCar->mBodyNode->getPosition());
    mCar->loadDestroyedModel();

    // Blast the stuff out of the car (renders it completely undriveable but since this
    // should only be called on dead cars thats not such a problem).
    // Yeah so turns out this just stuffs everything up and not in a good way.
    mCar->applyForce(mCar->mBodyNode, Ogre::Vector3(0, 500.0f, 0));
}

void Player::setAlive(bool pAlive)
{
	mAlive = pAlive;
}

bool Player::getAlive()
{
	return mAlive;
}

void Player::setGUID(RakNet::RakNetGUID playerGUID) {
	stringGUID = playerGUID.ToString();
}

std::string Player::getGUID(void) {
	return stringGUID;
}

RakNet::RakNetGUID Player::getPlayerGUID()
{
	return mPlayerGUID;
}

void Player::setPlayerGUID(RakNet::RakNetGUID playerGUID)
{
	mPlayerGUID = playerGUID;
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

// HEALTH RELATED FUNCTIONS

void Player::serverSaysHealthChangedTo(float newHP)
{
    if( this != GameCore::mPlayerPool->getLocalPlayer() )
    {
        this->hp = newHP;
        return;
    }

    OutputDebugString("Local player health changed\n");


    // update damage HUD

    float redLimit = (float) INITIAL_HEALTH / 4.0;
    float yelLimit = (float) INITIAL_HEALTH * 0.66;
    
    // the last quarter is red
    if (newHP <= redLimit)
    {
        if (this->hp > redLimit)
        {
            // set colour to red, as it was yellow previously

            // leave some components randomly yellow for a "cheat" damage system on demo day :D
            // part 0-body, 1-engine, 2-fl, 3-fr, 4-rl, 5-rr.
            // colour 0-green, 1-yellow, 2-red
            GameCore::mGui->updateDamage(0, 2-(rand()%2));
            GameCore::mGui->updateDamage(1, 2-(rand()%2));
            GameCore::mGui->updateDamage(2, 2-(rand()%2));
            GameCore::mGui->updateDamage(3, 2-(rand()%2));
            GameCore::mGui->updateDamage(4, 2-(rand()%2));
            GameCore::mGui->updateDamage(5, 2-(rand()%2));
        }
    }
    else if (newHP <= yelLimit)
    {
        if (this->hp > yelLimit)
        {
            // set colour to yellow, as it was green previously
            GameCore::mGui->updateDamage(0, 1-(rand()%2));
            GameCore::mGui->updateDamage(1, 1-(rand()%2));
            GameCore::mGui->updateDamage(2, 1-(rand()%2));
            GameCore::mGui->updateDamage(3, 1-(rand()%2));
            GameCore::mGui->updateDamage(4, 1-(rand()%2));
            GameCore::mGui->updateDamage(5, 1-(rand()%2));
        }
    }
    else
    {
        if (this->hp <= yelLimit)
        {
            // change colour back to green, its been red or yellow already
            GameCore::mGui->updateDamage(0, 0);
            GameCore::mGui->updateDamage(1, 0);
            GameCore::mGui->updateDamage(2, 0);
            GameCore::mGui->updateDamage(3, 0);
            GameCore::mGui->updateDamage(4, 0);
            GameCore::mGui->updateDamage(5, 0);
        }
    }
    
    this->hp = newHP;
}

void Player::lowDamageCallBack(std::string causedBy) {

}

void Player::midDamageCallBack(std::string causedBy) {

}

void Player::highDamageCallBack(std::string causedBy) {
}

void Player::applyHealthBonus()
{

}

int Player::getHP()
{
	return hp;
}

void Player::resetHP()
{
	//this->hp = INITIAL_HEALTH;
    serverSaysHealthChangedTo((float) INITIAL_HEALTH);
}

void Player::killPlayer(Player* causedBy)
{
	this->killPlayer();
	GameCore::mGameplay->markDeath(this,causedBy);
    mLastKiller = causedBy;

	if(this->getVIP())
	{
		causedBy->addToScore(5);
	}
	else
	{
		causedBy->addToScore(1);
	}

    InfoItem *spectate = new InfoItem( PLAYER_KILLED_OT, 0, 3 );
    GameCore::mGameplay->mInfoItems.push_back( spectate );
}

void Player::setOverlayElement(Ogre::OverlayElement* ole)
{
        mOLE = ole;
}