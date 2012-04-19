/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "CarCam.h"

enum TeamID;


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Contains the Player nodes and the related data.
 */
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
    void collisionTickCallback(btVector3 &hitPoint, float depth, Player *causedByPlayer);
    void applyHealthBonus (void);
    
    const char *getNickname (void) { return mNickname; }
    int getCarType (void) { return mCarType; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (char *szNick) { mNickname = strdup( szNick ); }
    void setHP (int newHP) { hp = newHP; }
	int	 getHP (void);
    void setVIP(bool newState) { mIsVIP = newState; }
    bool getVIP(void) { return mIsVIP; }
    void setTeam(TeamID newTeam) { mTeam = newTeam; if( mCar ) mCar->updateTeam(newTeam); }
    TeamID getTeam(void) { return mTeam; }

	void setGUID(RakNet::RakNetGUID playerGUID);
	std::string getGUID(void);

	//Overlay Element stuff for the big screen view
	void setOverlayElement (Ogre::OverlayElement* ole);
	Ogre::OverlayElement* getOverlayElement() { return this->mOLE; };

	//Just some getters and setters
	void setAlive(bool pAlive);
	bool getAlive();

	//Now have a "Kill" method that will also set the call backs
	void killPlayer();
	void Player::killPlayer(Player* causedBy);
	void resetHP();
    GameCamera* getCamera() { return mCamera; }
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


private:
    const float								   cameraRotationConstant;
	int										   hp;
    TeamID									   mTeam;
    char*									   mNickname;
    bool									   mIsVIP;
    bool									   mAlive;
    Car*									   mCar;
    CarSnapshot*							   mCarSnapshot;
	CarCam*									   mCarCam;
	Ogre::SceneNode*						   camNode;
	Ogre::SceneNode*						   camArmNode;
    CarType									   mCarType;
	Ogre::OverlayElement* mOLE;
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

    GameCamera*      mCamera;
	int roundScore;
	int gameScore;
};

#endif // #ifndef PLAYER_H
