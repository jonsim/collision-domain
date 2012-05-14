/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Car.h"
#include "InputState.h"
#include "Powerup.h"
#include "RakNetTypes.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Contains the Player nodes and the related data.
 */

enum PLAYER_STATE : int
{
    PLAYER_STATE_TEAM_SEL,
    PLAYER_STATE_SPAWN_SEL,
    PLAYER_STATE_WAIT_SPAWN,
    PLAYER_STATE_INGAME,
    PLAYER_STATE_SPECTATE,
};

struct PLAYER_DAMAGE_LOC
{
    float damageTL;
    float damageBL;
    float damageML;
    float damageTR;
    float damageBR;
    float damageMR;
};

class Player
{
 
public:
    Player (void);
    ~Player (void);
    void createPlayer (CarType carType, TeamID tid, ArenaID aid);
    void processControlsFrameEvent (InputState *userInput, Ogre::Real secondsSinceLastFrame, float targetPhysicsFrameRate);
    Car* getCar (void);
    void delCar() { if( mCar ) { delete mCar; mCar = NULL; } }
    //void collisionTickCallback (int damage, Player *causedByPlayer);
#if _WIN32    
    void collisionTickCallback(Ogre::Vector3 &hitPoint, Ogre::Real damage, unsigned int damageSection, int crashType, Player *causedByPlayer);
#else
    void collisionTickCallback(Ogre::Vector3 hitPoint, Ogre::Real &damage, int& damageSection, int& crashType, Player *&causedByPlayer);
#endif
        
    const char *getNickname (void) { return mNickname; }
    int getCarType (void) { return mCarType; }
    void setCarType( CarType t ) { mCarType = t; }

    PLAYER_STATE getPlayerState() { return mPlayerState; }
    void setPlayerState( PLAYER_STATE s ) { mPlayerState = s; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (char *szNick) { mNickname = strdup( szNick ); }
	int	 getHP (void);

	void setGUID(RakNet::RakNetGUID playerGUID);
	std::string getGUID(void);
	void setHP(int newHP) { hp = newHP;}
	void setOverlayElement (Ogre::OverlayElement* ole);
    void setSpawned (void); //Marks the car as spawned
    Ogre::OverlayElement* getOverlayElement (void);

	InputState* newInput;

	RakNet::RakNetGUID getPlayerGUID();
	void setPlayerGUID(RakNet::RakNetGUID playerGUID);

	//Just some getters and setters
	void setAlive(bool pAlive);
	bool getAlive();
    void setVIP(bool newState) { mIsVIP = newState; };
    bool getVIP(void) { return mIsVIP; };
    void setTeam(int newTeam) { mTeam = newTeam; };
    int  getTeam(void) { return mTeam; };

	//Now have a "Kill" method that will also set the call backs
	void killPlayer();
	RakNet::RakNetGUID playerGUID;
	void killPlayer(Player* causedBy);
	void resetHP();

	//Increase
	void addToScore(int amount);
	int getRoundScore();	
    void setRoundScore( int rs ) { this->roundScore = rs; }
    int getGameScore() { return this->gameScore; }
    void setGameScore( int gs ) { this->gameScore = gs; }
	void addToGameScore(int amount);
    int lastsenthp;
	bool isReady() { return mSpawned && mCar;}

    void cameraLookLeft(void);
	void cameraLookRight(void);
	void cameraLookBack(void);
	void revertCamera(void);

    void addPowerup( PowerupType type, RakNet::TimeMS endtime );

    PLAYER_DAMAGE_LOC damageLoc;
    float            rearDamageBoundary;
    float            frontDamageBoundary;

private:
    const float      cameraRotationConstant;
	int		         hp;
	int              initialHP;

    float            damageShareTL; 
    float            damageShareBL; 
    float            damageShareML; 
    float            damageShareTR; 
    float            damageShareBR;  
    float            damageShareMR;

    float            recalculateDamage(void);

    int              mTeam;
	bool		     mAlive;
    bool             mIsVIP;
	bool             mSpawned;
    PLAYER_STATE     mPlayerState;

    char*            mNickname;

    Car*             mCar;
    CarSnapshot*     mCarSnapshot;
    CarType          mCarType;

	RakNet::RakNetGUID mPlayerGUID;
	bool							  processingCollision;
	std::map<std::string, float>	  collisionDamages;
	std::map<std::string, btVector3>  collisionPositions;
	int							      numCollisionDataPoints;
	btVector3					      averageCollisionPoint;
								      
	std::string					      stringGUID;

    //std::map<PowerupType, RakNet::TimeMS> powerupTimers;
    RakNet::TimeMS powerupTimers[POWERUP_COUNT];

	int roundScore;
	int gameScore;
};

#endif // #ifndef PLAYER_H
