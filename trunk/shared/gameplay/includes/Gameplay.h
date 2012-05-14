/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InfoItem.h"
#include "ScoreBoard.h"
#include "HUD.h"
#include "Death.h"
#include "Team.h"
#include "SceneSetup.h"

#include <math.h>
#include <string>
#include "RakNetTypes.h"

/*-------------------- Some Game Values -------------*/
#define NUM_PLAYERS_TO_START 5
#define NUM_TOP_PLAYERS 5
#define NUM_TEAMS 2
#define NUM_ROUNDS 3

//#define SELECTED_GAME FFA_MODE
#define SELECTED_GAME FFA_MODE

/*-------------------- ENUM TYPES -------------------*/
// Represents a game mode. FFA = Free for all; TDM = Team deathmatch, KOTH = King of the hill.
enum GameMode {FFA_MODE, TDM_MODE, VIP_MODE};

class Player;
class InfoItem;
class Team;

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Gameplay
{
public:
								Gameplay();
                                ~Gameplay();
	bool						gameOver();
	bool						hasWon(TeamID teamID);
	void						checkIfGameOver();
	void						setGameMode(GameMode gameMode);
    GameMode                    getGameMode() { return mGameMode; }
	void						setNewVIP(TeamID teamID);
    void                        setNewVIP(TeamID teamID, Player* newVIP);
	void						setNewVIPs();
    bool                        addPlayer( RakNet::RakNetGUID playerid, TeamID requestedTeam );
    void                        playerQuit(Player *player);
    bool                        validateTeamChoice(TeamID requestedTeam);
    void						notifyDamage(Player* player);
	void						preparePlayers(); //Place the palyers in the correct place
	void						resetAllHP();
    void                        spawnPlayers();
	void						positionPlayers();
	void						startGame();
	void						drawInfo(); //Draws any info that we require
	void						setupOverlay();
	void						drawDeathInfo();
	void						initialize();
    Player*                     getRoundWinner();

	ScoreBoard*					mSB;
	std::vector<InfoItem*>		mInfoItems;
	int							numberOfTeams;
	bool						mGameActive; //True = Game underway, False = Game not yet started
    bool                        mCountDownActive;
	void						markDeath(Player* deadPlayer, Player* causedBy);
    void                        handleDeath(Player* deadPlayer, Player* causedBy);
	std::vector<DEATH*>			getDeathList();
	void						restartGame();
    void                        resetRoundScores();

    void                        setRoundNumber(int newRound) {roundNumber = newRound;}
    void                        incrementRoundNumber() { roundNumber++; }
    int                         getRoundNumber() { return roundNumber; }

    void                        generateGameOrder (void);
    void                        cycleGame( bool unload = true );
    void                        handleNewRound();
    Team*						getTeam(TeamID teamID);
    void						handleInfoItem(InfoItem* item, bool show);
    void                        forceRoundEnd();

    void                        setArenaID(ArenaID arenaID) { this->mArenaID = arenaID; }
    ArenaID                     getArenaID() { return this->mArenaID; }
    
private:
	//Methods
	bool						vipModeGameWon();
	
    Team*                       autoAssignTeam();
	void						printTeamStats();
	void						scheduleCountDown();

    //Winner text stuff
    void                        createWinnerTextOverlay();
    void                        showWinnerText(Player* winningPlayer, bool round);
    void                        hideWinnerText();

    //Game type text suff
    void                        showGameTypeText();
    void                        hideGameTypeText();

    //Variables
    Team*                       mTeams[NUM_TEAMS];
	GameMode					mGameMode;
	
	Ogre::OverlayContainer*		olContainer;
	

	HUD*						mHUD;
	std::vector<DEATH*>			deathList;
	void						calculateRoundScores();
	Player*						topPlayers[NUM_TOP_PLAYERS];

    int                         roundNumber;

    //Winner text stuff
    bool                        wtInitalised;
    Ogre::Overlay               *wtOverlay;
    Ogre::OverlayContainer      *wtContainer;
    Ogre::OverlayElement        *textAreaT1;

    Ogre::OverlayElement        *oleGameMode;

#ifdef COLLISION_DOMAIN_SERVER
    // Randomised game modes and arenas.
    GameMode                    mGamemodeOrder[3];
    ArenaID                     mArenaOrder[3];
#endif

    // Current arena
    ArenaID                     mArenaID;
};


#endif
