/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "InfoItem.h"

#include <math.h>
#include <string>
#include "RakNetTypes.h"

/*-------------------- ENUM TYPES -------------------*/
enum
{
	FREE_FOR_ALL_MODE,
	TEAM_FREE_FOR_ALL_MODE,
	KING_OF_THE_HILL_MODE,
	VIP_MODE
};

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Gameplay
{
public:
								Gameplay();
	void						setNumberOfTeams(int num);
	Ogre::Real					getScorePercentage(std::string identifier);
	int							getScoreValue(std::string identifier);
	Team*						createTeam(std::string teamName);
	void						addPlayerToTeam(Team* team, Player* player);
	bool						gameOver();
	bool						hasWon(Team* team);
	Team*						checkIfGameOver();
	void						setGameplayMode(int gameplayMode);
	Player*						setNewVIP(Team* team);
	void						setAllNewVIP();
	Team*						declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
	void						notifyDamage(Player* player);
	void						preparePlayers(); //Place the palyers in the correct place
	void						resetAllHP();
	void						positionPlayers();
	void						startGame();
	void						drawInfo(); //Draws any info that we require
	void						setupOverlay();

	std::vector<InfoItem*>		mInfoItems;
private:
	//Methods
	bool						vipModeGameWon();
	Team*						getTeamToJoin();
	void						printTeamStats();
	void						scheduleCountDown();
	//Variabels
	int							numberOfTeams;
	std::vector<Team*>			teams;
	int							mGameplayMode;
	
	Ogre::OverlayContainer* olContainer;
	void						handleInfoItem(InfoItem* item, bool show);
};


#endif