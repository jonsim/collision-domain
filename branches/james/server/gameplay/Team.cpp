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