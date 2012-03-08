/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef TEAM_H
#define TEAM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

#include <math.h>
#include <string>

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Team
{
public:
							Team(std::string teamName_P, int teamNumber);
	void					addPlayer(Player *player);
	std::vector<Player*>	getPlayers();
	std::string				getName();
	Player*					getRandomPlayer();
	Player*					setNewVIP(Player* player);
	int						getTeamSize();
	int						getTotalTeamHP();
private:
	std::vector<Player*>	players;
    int                     teamNumber;
	std::string				teamName;
	Player*					vipPlayer;
};


#endif