/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
//#include "InfoItem.h"
//#include "ScoreBoard.h"
#include "HUD.h"
#include "Death.h"

#include <math.h>
#include <string>
#include "RakNetTypes.h"

/*-------------------- Some Game Values -------------*/
#define NUM_PLAYERS_TO_START 5
#define NUM_TOP_PLAYERS 5
#define NUM_TEAMS 2
#define NUM_ROUNDS 3

#define SELECTED_GAME TDM_MODE

/*-------------------- ENUM TYPES -------------------*/
// Represents a game mode. FFA = Free for all; TDM = Team deathmatch, KOTH = King of the hill.
enum GameMode {FFA_MODE, TDM_MODE, VIP_MODE};
enum TeamID   {NO_TEAM = 0, BLUE_TEAM = 1, RED_TEAM = 2};

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
	void						markDeath(Player* deadPlayer, Player* causedBy);
	std::vector<DEATH*>			getDeathList();
	void						restartGame();
    void                        resetRoundScores();

    void                        setRoundNumber(int newRound) {roundNumber = newRound;}
    void                        incrementRoundNumber() { roundNumber++; }
    int                         getRoundNumber() { return roundNumber; }
private:
	//Methods
	bool						vipModeGameWon();
	Team*						getTeam(TeamID teamID);
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
	void						handleInfoItem(InfoItem* item, bool show);

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
};


#endif