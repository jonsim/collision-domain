/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "CarCam.h"
#include "Camera.h"
#include "Team.h"
#include "Car.h"
#include "SimpleCoupeCar.h"
#include "TruckCar.h"
#include "SmallCar.h"
#include "InputState.h"


// Uncomment this definition to colour people's nameplates based on their team.
//#define TEAM_COLOURED_NAMEPLATES


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

enum PowerupBoardType : int
{
    POWERUP_BOARD_HEAVY,
    POWERUP_BOARD_LIGHT,
    POWERUP_BOARD_HEALTH,
    POWERUP_BOARD_SPEED
};

class PowerupBoardState
{
public:
    PowerupBoardState()
    {
        PowerupBoardState(POWERUP_BOARD_HEAVY, -1.0f);
    }

    PowerupBoardState(PowerupBoardType type, float fadeOutInSeconds)
      : fadeOutInSeconds(fadeOutInSeconds < 0 ? 0.001f : fadeOutInSeconds),
        remaining       (fadeOutInSeconds <= 0 ? -1.0f : fadeOutInSeconds),
        type            (type)
    {}
    
    // performs the time elapsed and returns true if the powerup has finished on this specific call
    bool timeElapsed(Ogre::Billboard *modify, float elapsedSeconds)
    {
        if (remaining < -0.001)
        {
            return false;
        }

        remaining -= elapsedSeconds;
        float alpha = 0.0f;

        if (remaining > 0)
        {
            // display the powerup between 0.1 and 1.0,
            // jump from 0.1 to 0.0 as its hard to tell in that range
            alpha = remaining / fadeOutInSeconds;
            if (alpha < 0.0f)
            {
                alpha = 0.0f;
            }

            modify->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, alpha));
            return false;
        }
        else
        {
            finishNow(modify);
            return true;
        }
    }

    void finishNow(Ogre::Billboard *modify)
    {
        remaining = -1;
        modify->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f));
    }

    bool isFinished()
    {
        return remaining < -0.001;
    }

    PowerupBoardType getType()
    {
        return type;
    }

    bool isType(PowerupBoardType isType)
    {
        if ( ( isType == POWERUP_BOARD_HEAVY && type == POWERUP_BOARD_LIGHT )
            || ( isType == POWERUP_BOARD_LIGHT && type == POWERUP_BOARD_HEAVY ) )
        {
            return true;
        }
        else
        {
            return isType == type;
        }
    }

    float getRemainingTime()
    {
        return remaining;
    }

private:
    float fadeOutInSeconds;
    float remaining;
    PowerupBoardType type;
};



class Player
{
public:
    Player (bool isAI);
    ~Player (void);
    void createPlayer (CarType carType, TeamID tid, ArenaID aid);
    void attachCamera (Ogre::Camera* cam);
    void processControlsFrameEvent (InputState *userInput, Ogre::Real secondsSinceLastFrame);
    void updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth, float time, GameCamera *gameCamera);
    void updateLocalGraphics (void);
    void updateGlobalGraphics (Ogre::Real secondsSinceLastFrame);
	float getCameraYaw (void);
    Car* getCar (void);
    void delCar()
    {
        if( mCar )
        {
            mCar->mBodyNode->detachAllObjects();
            delete mCar;
            mCar = NULL;
        }
    }
#if _WIN32
    void collisionTickCallback(Ogre::Vector3 &hitPoint, Ogre::Real damage, unsigned int damageSection, int crashType, Player *causedByPlayer);
#else
    void collisionTickCallback(Ogre::Vector3 hitPoint, Ogre::Real &damage, int& damageSection, int &crashType, Player *&causedByPlayer);
#endif
    void applyHealthBonus (void);
    void frameEvent(float time);
    
    const char *getNickname (void) { return mNickname; }
    CarType getCarType (void) { return mCarType; }

    PLAYER_STATE getPlayerState() { return mPlayerState; }
    void setPlayerState( PLAYER_STATE s ) { mPlayerState = s; }

    // Probably a better alternative to strdup (could use std::string but I've never been a fan, I like C strings :D )
    void setNickname (const char *szNick);
    void setHP (int newHP) { hp = newHP; }
	int	 getHP (void);
    void setVIP(bool newState) { mIsVIP = newState; }
    bool getVIP(void) { return mIsVIP; }
    void setTeam(TeamID newTeam);
    TeamID getTeam(void) { return mTeam; }
    bool isAI();

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
    void processDamage( PLAYER_DAMAGE_LOC damageIn );

	RakNet::RakNetGUID getPlayerGUID();
	void setPlayerGUID(RakNet::RakNetGUID playerGUID);

	CarSnapshot *mSnapshots;

	void pushBackNewPowerupBoard(PowerupBoardType type, float fadeOutInSeconds);
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
    float            rearDamageBoundary;
    float            frontDamageBoundary;

    void addPowerup( PowerupType type, RakNet::TimeMS endtime );


private:
    void reLayoutPowerupBoards();

    const float								   cameraRotationConstant;
	int										   hp;
    TeamID									   mTeam;
    char*									   mNickname;
    bool                                       mIsAI;
    bool									   mIsVIP;
    bool									   mAlive;
    Player*                                    mLastKiller;
    RakNet::TimeMS                             mTimeLastKilled;
    PLAYER_STATE                               mPlayerState;
    Car*									   mCar;
    CarSnapshot*							   mCarSnapshot;
	CarCam*									   mCarCam;
	Ogre::SceneNode*						   camNode;
	Ogre::SceneNode*						   camArmNode;
    CarType									   mCarType;

    #define NUM_POWERUP_BOARDS 3

    Ogre::BillboardSet*                        mPowerupBoards[NUM_POWERUP_BOARDS];
    Ogre::Billboard*                           mPowerupBars  [NUM_POWERUP_BOARDS];
    PowerupBoardState                          mPowerupStates[NUM_POWERUP_BOARDS];


    Ogre::BillboardSet*                        mBoards;
    Ogre::BillboardSet*                        mBacks;
    Ogre::Billboard*                           mHealthbar;
    Ogre::Billboard*                           mHealthBg;
    Ogre::MovableText*                         mNametag;

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

    float            damageShareTL; 
    float            damageShareBL; 
    float            damageShareML; 
    float            damageShareTR; 
    float            damageShareBR;  
    float            damageShareMR; 

    //std::map<PowerupType, RakNet::TimeMS> powerupTimers;
    RakNet::TimeMS powerupTimers[POWERUP_COUNT];

	int roundScore;
	int gameScore;

    bool mFirstLaunch;
    int crashCount;
    int camShakeFrames;
};

#endif // #ifndef PLAYER_H
