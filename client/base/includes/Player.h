/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "CarCam.h"
#include "Team.h"
#include "Car.h"
#include "InputState.h"


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

class Player
{
public:
    Player (void);
    ~Player (void);
    void createPlayer (CarType carType, TeamID tid);
    void attachCamera (Ogre::Camera* cam);
    void processControlsFrameEvent (InputState *userInput, Ogre::Real secondsSinceLastFrame);
    void updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth, float time);
    void updateLocalGraphics (void);
    void updateGlobalGraphics (bool isForward, Ogre::Real secondsSinceLastFrame);
	float getCameraYaw (void);
    Car* getCar (void);
    void delCar() { if( mCar ) { delete mCar; mCar = NULL; } }
    void collisionTickCallback(btVector3 &hitPoint, float depth, Player *causedByPlayer);
    void applyHealthBonus (void);
    
    const char *getNickname (void) { return mNickname; }
    int getCarType (void) { return mCarType; }

    PLAYER_STATE getPlayerState() { return mPlayerState; }
    void setPlayerState( PLAYER_STATE s ) { mPlayerState = s; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (char *szNick) { mNickname = strdup( szNick ); }
    void setHP (int newHP) { hp = newHP; }
	int	 getHP (void);
    void setVIP(bool newState) { mIsVIP = newState; }
    bool getVIP(void) { return mIsVIP; }
    void setTeam(TeamID newTeam);
    TeamID getTeam(void) { return mTeam; }

	void setGUID(RakNet::RakNetGUID playerGUID);
	std::string getGUID(void);

	//Just some getters and setters
	void setAlive(bool pAlive);
	bool getAlive();

	//Now have a "Kill" method that will also set the call backs
	void killPlayer();
	void killPlayer(Player* causedBy);
	void resetHP();
    void serverSaysHealthChangedTo(float newHP);

	RakNet::RakNetGUID getPlayerGUID();
	void setPlayerGUID(RakNet::RakNetGUID playerGUID);

	CarSnapshot *mSnapshots;
    Player* mLastKiller;

	
	void addToScore(int amount);
	int getRoundScore();	
    void setRoundScore( int rs ) { this->roundScore = rs; }
    int getGameScore() { return this->gameScore; }
    void setGameScore( int gs ) { this->gameScore = gs; }
	void addToGameScore(int amount);
	void lowDamageCallBack(std::string causedBy);
	void midDamageCallBack(std::string causedBy);
	void highDamageCallBack(std::string causedBy);

	void cameraLookLeft(void);
	void cameraLookRight(void);
	void cameraLookBack(void);
	void revertCamera(void);
	void cycleCameraView(void);

    void angleTest(void);


private:
    const float								   cameraRotationConstant;
	int										   hp;
    TeamID									   mTeam;
    char*									   mNickname;
    bool									   mIsVIP;
    bool									   mAlive;
    PLAYER_STATE                               mPlayerState;
    Car*									   mCar;
    CarSnapshot*							   mCarSnapshot;
	CarCam*									   mCarCam;
	Ogre::SceneNode*						   camNode;
	Ogre::SceneNode*						   camArmNode;
    CarType									   mCarType;

	int                                        numCameraViews;
	int										   cameraView;
	btVector3                                  cameraViews[3];
	btVector3                                  cameraLookViews[3];

	/*bool									   processingCollision;
	std::map<std::string, std::vector<float> >			   collisionDamages;
	std::map<std::string, std::vector<float> >::iterator	   collisionDamagesItr;
	std::map<std::string, std::vector<btVector3> >		   collisionPositions;
	std::map<std::string, std::vector<btVector3> >::iterator collisionPositionsItr;

	std::vector<btVector3>                    *currentPositions;
	std::vector<float>                        *currentDamages;*/

	int									       numCollisionDataPoints;
	//btVector3							       averageCollisionPoint;
	std::string							   stringGUID;
	int										   lowDamageSpeed;
	int                                        mediumDamageSpeed;
	int                                        highDamageSpeed;
	RakNet::RakNetGUID mPlayerGUID;

    float initialHP;
	float backRightDamageShare;
	float backLeftDamageShare;  
	float frontLeftDamageShare;
	float frontRightDamageShare;



	int roundScore;
	int gameScore;

    bool mFirstLaunch;
};

#endif // #ifndef PLAYER_H
