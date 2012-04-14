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
							Team(TeamID tid);
	void					addPlayer(Player *player);
	std::vector<Player*>	getPlayers();
	Player*					getRandomPlayer();
	Player*					setNewVIP(Player* player);
	Player*					getVIP();
	int						getTeamSize();
	int						getTotalTeamHP();
private:
	std::vector<Player*>	players;
    TeamID                  teamID;
	Player*					vipPlayer;
};


#endif