/**
 * @file	Player.cpp
 * @brief 	Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"
#include "GameCore.h"
#include "SimpleCoupeCar.h"
#include "TruckCar.h"
#include "SmallCar.h"



#define MAX_DAMAGE 400 // used cap damage for individual crashes so that deformations are more managable
#define BIG_CRASH_THRESHOLD 80
/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (void)
  : cameraRotationConstant(0.08f),
    mSpawned(false),
    mAlive(false),
    mIsVIP(false),
    mTeam(0),
    mCarSnapshot(NULL),
    newInput(NULL),
    mCar(NULL)
{
    // PlayerState state configures constants and zeros values upon creation.
    for( int i = 0; i < POWERUP_COUNT; i ++ )
        powerupTimers[i] = 0;
}


/// @brief   Deconstructor.
Player::~Player (void)
{
	if( mCar )
    {
		delete( mCar );
        mCar = NULL;
    }
}


/// @brief  Creates and positions the 3D player object (and space for a camera).
/// @param  sm  The SceneManager to which the 3D player object is attached.
/// @param  t   The car model to load as the player object.
/// @param  s   The texture to apply to the car model.
/// @param  physicsCore   The class containing the physics world.
void Player::createPlayer (CarType carType, TeamID tid, ArenaID aid)
{
    if( mCar )
        delCar();

    mCarType = carType;

    switch (carType)
    {
    case CAR_BANGER:
        mCar = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
        break;
    case CAR_SMALL:
        mCar = (Car*) new SmallCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
        break;
    case CAR_TRUCK:
        mCar = (Car*) new TruckCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
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

    damageShareTL = 0.1f;
    damageShareBL = 0.2f;
    damageShareML = 0.2f;
    damageShareTR = 0.1f;
    damageShareBR = 0.2f;
    damageShareMR = 0.2f;

    damageLoc.damageTL = 0.f;
    damageLoc.damageBL = 0.f;
    damageLoc.damageML = 0.f;
    damageLoc.damageTR = 0.f;
    damageLoc.damageBR = 0.f;
    damageLoc.damageMR = 0.f;
}


/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
///         In total this will even out to 60 calls per second :-)
/// @param  damage   0 if no damage was done to this player in the collision, else 1.
#if _WIN32
    void Player::collisionTickCallback(Ogre::Vector3 &hitPoint, Ogre::Real damage, unsigned int damageSection, int crashType, Player *causedByPlayer) {
#else
    void Player::collisionTickCallback(Ogre::Vector3 hitPoint, Ogre::Real& damage, int& damageSection, int &crashType, Player *&causedByPlayer) {
#endif
    switch(damageSection) {
        case 0 :
            damageLoc.damageTL += damage;
            break;
        case 1 :
            damageLoc.damageTR += damage;
            break;
        case 2 :
            damageLoc.damageML += damage;
            break;
        case 3 :
            damageLoc.damageMR += damage;
            break;
        case 4 :
            damageLoc.damageBL += damage;
            break;
        case 5 :
            damageLoc.damageBR += damage;
            break;
        default:
            // dont do that you stupid cunt
            break;
    }

	if((GameCore::mGameplay->mGameActive && mAlive)) {
		hp = recalculateDamage();
		GameCore::mGameplay->notifyDamage(this);
		//Force health to never drop below 0
		if(hp <= 0) {
			hp = 0;
			this->killPlayer(causedByPlayer);
		}
	}
}
/*
void Player::reduceDamage(float byAmount) {
    if(byAmount > 0.0f) {
        float jamieisgay = byAmount * 0.1667f;
        damageLoc.damageTL = damageLoc.damageTL > jamieisgay ? damageLoc.damageTL - jamieisgay : 0;
        damageLoc.damageBL = damageLoc.damageBL > jamieisgay ? damageLoc.damageBL - jamieisgay : 0;
        damageLoc.damageML = damageLoc.damageML > jamieisgay ? damageLoc.damageML - jamieisgay : 0;
        damageLoc.damageTR = damageLoc.damageTR > jamieisgay ? damageLoc.damageTR - jamieisgay : 0;
        damageLoc.damageBR = damageLoc.damageBR > jamieisgay ? damageLoc.damageBR - jamieisgay : 0;
        damageLoc.damageMR = damageLoc.damageMR > jamieisgay ? damageLoc.damageMR - jamieisgay : 0;
    }
}
*/
float Player::recalculateDamage(void) {
	return initialHP - (
		( damageLoc.damageTL * damageShareTL ) +
        ( damageLoc.damageBL * damageShareBL ) +
        ( damageLoc.damageML * damageShareML ) +
        ( damageLoc.damageTR * damageShareTR ) +
        ( damageLoc.damageBR * damageShareBR ) +
        ( damageLoc.damageMR * damageShareMR )
    );
}

void Player::cameraLookLeft(void) {
	//OutputDebugString("look left\n");
}

void Player::cameraLookRight(void) {
	//OutputDebugString("look right\n");
}

void Player::cameraLookBack(void) {
	//OutputDebugString("look back\n");
}

void Player::revertCamera(void) {
	//OutputDebugString("revert\n");
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
    if( !this->getCar() )
        return;

    // Remove any powerups that have expired
    for( int i = 0; i < POWERUP_COUNT; i ++ )
    {
        if( powerupTimers[i] != 0 && RakNet::GreaterThan( RakNet::GetTimeMS(), powerupTimers[i] ) )
        {
            switch( i )
            {
            case POWERUP_MASS:
                getCar()->resetMass();
                getCar()->resetEngineForce();
                GameCore::mGui->outputToConsole( "Player '%s' mass effect ended.\n", getNickname() );
                break;
            case POWERUP_SPEED:
                getCar()->resetEngineForce();
                break;
            }

            powerupTimers[i] = 0;
        }
    }

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
    setPlayerState( PLAYER_STATE_SPECTATE );
	//GameCore::mNetworkCore->sendPlayerDeath(this);

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
    //Send via chat a killed message
    char killMessage[200];
    sprintf(killMessage,"%s killed %s\n",causedBy->getNickname(),this->getNickname());
    GameCore::mNetworkCore->sendChatMessage( killMessage );
    //Send to the server console the kill message
    GameCore::mGui->outputToConsole(killMessage);

	GameCore::mGameplay->markDeath(this,causedBy);
    GameCore::mNetworkCore->sendPlayerDeath(this, causedBy);
    GameCore::mGameplay->handleDeath(this,causedBy);
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

void Player::addPowerup( PowerupType type, RakNet::TimeMS endtime )
{
    powerupTimers[type] = endtime;
}
