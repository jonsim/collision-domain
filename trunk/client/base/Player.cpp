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

#define MAX_DAMAGE 400
#define BIG_CRASH_THRESHOLD 80
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

	numCameraViews = 3;
	cameraView = 0;

	cameraViews[0]     = btVector3( 0.f, 5.f,  -10.f ); //default
	cameraViews[1]     = btVector3( 0.f, 3.8f, -9.f ); //lower+closer
	cameraViews[2]     = btVector3( 0.f, 2.8f, 0.f ); //windshield

	cameraLookViews[0] = btVector3( 0.f, 0.f, 3.0f );
	cameraLookViews[1] = btVector3( 0.f, 3.8f, 3.0f );
	cameraLookViews[2] = btVector3( 0.f, 2.8f, 8.f );

    std::stringstream ss;
    ss << "car type: " << mCarType << "\n";
    ss << "our team: " << mTeam << "\n";
    OutputDebugString(ss.str().c_str());

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
    if (isLocalPlayer) {
        mCar->louderLocalSounds();
        GameCore::mGui->setupDamageDisplay(carType, tid);
    }

    hp                    = INITIAL_HEALTH;
    initialHP             = INITIAL_HEALTH;
	backRightDamageShare  = 0.3f;
	backLeftDamageShare   = 0.3f;
	frontLeftDamageShare  = 0.2f;
	frontRightDamageShare = 0.2f;

	roundScore = 0;
    mCar->attachCollisionTickCallback(this);
    
    mCar->moveTo(btVector3(0,0.5,0));

    //GameCore::mGui->updateDamage(0, 2-(rand()%2));
}

void Player::angleTest(void) {
    Ogre::Vector3 lineStart = this->getCar()->mBodyNode->convertLocalToWorldPosition(Ogre::Vector3(0, -30, 0));
    Ogre::Vector3 lineEnd   = this->getCar()->mBodyNode->convertLocalToWorldPosition(
        Ogre::Vector3(
            Ogre::Math::Cos(0) * 100,
            -30,
            Ogre::Math::Sin(0) * 100
        )
    );

    GameCore::mClientGraphics->mMeshDeformer->drawLine(GameCore::mSceneMgr, lineStart, lineEnd);
}
/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
/// @param  hitPoint		Location of the collision point on the collision mesh - in world coordinates
/// @param  speed			The speed of the impact in the direction of the normal to the collision point
/// @param  causedByPlayer	Pointer to the other player in the collision.
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

	

	// differentiate between differnt collision types
	if(totalDamage < BIG_CRASH_THRESHOLD && (p1Speed > 40 || p2Speed > 40)) {
		//OutputDebugString("Gleam\n");
	} else if(totalDamage < BIG_CRASH_THRESHOLD && (p1Speed < 40 && p2Speed < 40)) {
		//OutputDebugString("Bump\n");
	} else if(totalDamage >= BIG_CRASH_THRESHOLD) {
		//OutputDebugString("Bang\n");
		GameCore::mClientGraphics->mMeshDeformer->collisonDeform(this->getCar()->mBodyNode, (Ogre::Vector3)hitPoint, damageToThis);
	}
}


void Player::cameraLookLeft(void) {
	OutputDebugString("look left\n");
}

void Player::cameraLookRight(void) {
	OutputDebugString("look right\n");
}

void Player::cameraLookBack(void) {

}

void Player::revertCamera(void) {

}

void Player::cycleCameraView(void) {

	cameraView = (++cameraView) & numCameraViews;

	GameCore::mClientGraphics->mGameCam->setOffset( cameraViews[cameraView] );
    GameCore::mClientGraphics->mGameCam->setLookOffset( cameraLookViews[cameraView] );

    
	switch(cameraView) {
		case 0 :
		case 1 :
			GameCore::mClientGraphics->mGameCam->setCamType( CAM_CHASE );
			break;
		case 2 :
			GameCore::mClientGraphics->mGameCam->setCamType( CAM_FIXED );
			break;
	}
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

    std::stringstream ss;
    ss << "localplayer health changed to " << newHP << "\n";

    OutputDebugString(ss.str().c_str());


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
            //GameCore::mGui->updateDamage(0, 2-(rand()%2));
            //GameCore::mGui->updateDamage(1, 2-(rand()%2));
            //GameCore::mGui->updateDamage(2, 2-(rand()%2));
            //GameCore::mGui->updateDamage(3, 2-(rand()%2));
            //GameCore::mGui->updateDamage(4, 2-(rand()%2));
            //GameCore::mGui->updateDamage(5, 2-(rand()%2));
        }
    }
    else if (newHP <= yelLimit)
    {
        if (this->hp > yelLimit)
        {
            // set colour to yellow, as it was green previously
            //GameCore::mGui->updateDamage(0, 1-(rand()%2));
            //GameCore::mGui->updateDamage(1, 1-(rand()%2));
            //GameCore::mGui->updateDamage(2, 1-(rand()%2));
            //GameCore::mGui->updateDamage(3, 1-(rand()%2));
            //GameCore::mGui->updateDamage(4, 1-(rand()%2));
            //GameCore::mGui->updateDamage(5, 1-(rand()%2));
        }
    }
    else
    {
        if (this->hp <= yelLimit)
        {
            // change colour back to green, its been red or yellow already
            //GameCore::mGui->updateDamage(0, 0);
            //GameCore::mGui->updateDamage(1, 0);
            //GameCore::mGui->updateDamage(2, 0);
            //GameCore::mGui->updateDamage(3, 0);
            //GameCore::mGui->updateDamage(4, 0);
            //GameCore::mGui->updateDamage(5, 0);
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
    GameCore::mGameplay->handleDeath(this,causedBy);
    mLastKiller = causedBy;
    InfoItem *spectate = new InfoItem( PLAYER_KILLED_OT, 0, 3 );
    GameCore::mGameplay->mInfoItems.push_back( spectate );
}

void Player::setOverlayElement(Ogre::OverlayElement* ole)
{
        mOLE = ole;
}