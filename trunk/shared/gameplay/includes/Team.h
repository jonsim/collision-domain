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

class Player;

enum TeamID   {NO_TEAM = 0, BLUE_TEAM = 1, RED_TEAM = 2};

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Team
{
public:
							Team(TeamID tid);
	void					addPlayer(Player *player);
    void                    delPlayer(Player *player);
	std::vector<Player*>	getPlayers();
	Player*					getRandomPlayer();
	Player*					setNewVIP(Player* player);
	Player*					getVIP();
    void                    removeVIP();
	int						getTeamSize();
	int						getTotalTeamHP();
private:
	std::vector<Player*>	players;
    TeamID                  teamID;
	Player*					vipPlayer;
};


#endif
