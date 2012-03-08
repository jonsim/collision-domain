/**
* @file		Team.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

Team::Team(std::string teamName_P, int teamNumber)
{
	Team::teamName = teamName_P;
    Team::teamNumber = teamNumber;
}

void Team::addPlayer(Player *player)
{
    player->getCar()->updateTeam(teamNumber, false);
    char bob[64];
    sprintf(bob, "team number = %d\n", teamNumber);
    OutputDebugString(bob);
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
		OutputDebugString("Unable to fetch player, No players in team\n");
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

int Team::getTotalTeamHP()
{
	int totalHP = 0;
	std::vector<Player*>::iterator itr;
	for(itr = players.begin(); itr<players.end(); ++itr)
	{
		Player* tmpPlayer = *itr;
		totalHP += tmpPlayer->getHP();
	}
	return totalHP;
}