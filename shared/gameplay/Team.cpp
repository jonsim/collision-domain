/**
* @file		Team.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

/// @brief Constructor.
/// @param teamName_P   The name for this team
/// @param teamNumber   The number which uniquely identifies this team. Should be >= 1
///                     (0 is reserved for no-team or free-for-all).
Team::Team(std::string teamName_P, int teamNumber)
{
	Team::teamName = teamName_P;
    Team::teamNumber = teamNumber;
}

void Team::addPlayer(Player *player)
{
    player->setTeam(teamNumber);
#ifdef COLLISION_DOMAIN_CLIENT
    player->getCar()->updateTeam(teamNumber);
#endif
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
        player->setVIP(true);
		vipPlayer = player;
		#ifdef COLLISION_DOMAIN_SERVER
			GameCore::mNetworkCore->declareNewVIP(vipPlayer);
		#endif
		return player;
	}
	else
	{
		return NULL;
	}
}

Player* Team::getVIP()
{
	return vipPlayer;
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