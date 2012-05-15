/**
* @file		Team.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Team.h"
#include "Player.h"
#include "GameCore.h"

/// @brief Constructor.
/// @param teamName_P   The name for this team
/// @param teamNumber   The number which uniquely identifies this team. Should be >= 1
///                     (0 is reserved for no-team or free-for-all).
Team::Team(TeamID tid)
{
	Team::teamID = tid;
    vipPlayer = NULL;
}

void Team::addPlayer(Player *player)
{
    player->setTeam(teamID);
#ifdef COLLISION_DOMAIN_CLIENT
    player->getCar()->updateTeam(teamID);
#endif
	players.push_back(player);
}

void Team::delPlayer(Player *player)
{
    std::vector<Player*>::iterator itr = find(players.begin(), players.end(), player);
    if(itr != players.end())
        players.erase(itr);
}

std::vector<Player*> Team::getPlayers()
{
	return Team::players;
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
		//OutputDebugString("Unable to fetch player, No players in team\n");
		return NULL;
	}
}

Player* Team::setNewVIP(Player* player)
{
	if(player!=NULL)
	{
		//OutputDebugString("Set new VIP player\n");
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

void Team::removeVIP()
{
    if(vipPlayer != NULL)
    {
        vipPlayer->setVIP(false);
        vipPlayer = NULL;
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
