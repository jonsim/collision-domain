/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef TEAM_H
#define TEAM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include <math.h>
#include <string>
#include "Player.h"

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Team
{
public:
							Team(std::string teamName_P);
	void					addPlayer(Player *player);
	std::vector<Player*>	getPlayers();
	std::string				getName();
	Player*					getRandomPlayer();
	Player*					setNewVIP(Player* player);
	int						getTeamSize();
private:
	std::vector<Player*>	players;
	std::string				teamName;
	Player*					vipPlayer;
};


#endif