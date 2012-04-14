/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef TEAM_H
#define TEAM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"

#include <math.h>
#include <string>

class Player;

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
	Player*					getVIP();
	int						getTeamSize();
	int						getTotalTeamHP();
private:
	std::vector<Player*>	players;
    int                     teamNumber;
	std::string				teamName;
	Player*					vipPlayer;
};


#endif