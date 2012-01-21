/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include <math.h>
#include <string>
#include "Team.h"
#include "Player.h"
#include "RakNetTypes.h"
#include "NetworkCore.h"

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
	void					setNumberOfTeams(int num);
	Ogre::Real				getScorePercentage(std::string identifier);
	int						getScoreValue(std::string identifier);
	Team*					createTeam(std::string teamName);
	void					addPlayerToTeam(Team* team, Player* player);
	bool					gameOver();
	bool					hasWon(Team* team);
	void					setGameplayMode(int gameplayMode);
	Player*					setNewVIP(Team* team);
	void					setAllNewVIP();
	Team*					declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
private:
	//Methods
	bool					vipModeGameWon();
	Team*					getTeamToJoin();

	//Variabels
	int						numberOfTeams;
	std::vector<Team*>		teams;
	int						mGameplayMode;
	
};


#endif