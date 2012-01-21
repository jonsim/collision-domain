/**
* @file		Team.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "includes\Team.h"

Team::Team(std::string teamName_P)
{
	Team::teamName = teamName_P;
}

void Team::addPlayer(Player *player)
{
	players.push_back(player);
}

std::vector<Player*> Team::getPlayers()
{
	return Team::players;
}

std::string Team::getName()
{
	return Team::teamName;
}

Player*	Team::getRandomPlayer()
{
	if(players.size() > 0)
	{
		int pickNum = rand() % players.size();
		return players[pickNum];
	}
	else
	{
		return NULL;
	}
}

Player* Team::setNewVIP(Player* player)
{
	if(player!=NULL)
	{
		OutputDebugString("Set new VIP player\n");
		vipPlayer = player;
		return player;
	}
	else
	{
		return NULL;
	}
}

int Team::getTeamSize()
{
	return players.size();
}