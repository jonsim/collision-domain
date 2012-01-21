/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Gameplay.h"


Gameplay::Gameplay()
{

}

void Gameplay::setNumberOfTeams(int num)
{
	numberOfTeams = num;
}

Team* Gameplay::createTeam(std::string teamName)
{
	Team* tmpTeam = new Team(teamName);
	teams.push_back(tmpTeam);
	return tmpTeam;
}

Ogre::Real Gameplay::getScorePercentage(std::string identifier)
{
	return 0.0;
}

int	Gameplay::getScoreValue(std::string identifier)
{
	return 0;
}

void Gameplay::addPlayerToTeam(Team* team, Player* player)
{
	team->addPlayer(player);
}

bool Gameplay::gameOver()
{
	if(mGameplayMode == VIP_MODE) {
		vipModeGameWon();
	}
	return false;
}

bool Gameplay::hasWon(Team* team)
{
	return false;
}

void Gameplay::setGameplayMode(int gameplayMode)
{
	mGameplayMode = gameplayMode;
}

bool Gameplay::vipModeGameWon()
{
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{

	}
	return false;
}

Player* Gameplay::setNewVIP(Team* team)
{
	return team->setNewVIP(team->getRandomPlayer());
}

void Gameplay::setAllNewVIP()
{
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* team = *itr;
		team->setNewVIP(team->getRandomPlayer());
	}
}

Team* Gameplay::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = NetworkCore::mPlayerPool->getPlayer(playerid);
	Team* teamToJoin = getTeamToJoin();
	teamToJoin->addPlayer(tmpPlayer);
	OutputDebugString("Joined player to team");
	return teamToJoin;
}

//Gets which team makes sense to join (Aims to balance)
Team* Gameplay::getTeamToJoin()
{
	Team*	lowestTeam;
	int		lowestNumOfPlayers;

	//Check to see that there is some teams to join
	if(teams.size() > 0)
	{
		//Set initial size to an actual value. Who knows it might be the correct value
		lowestTeam = teams[0];
		lowestNumOfPlayers = lowestTeam->getTeamSize();

		//Loop through all teams and find the one with the lowest value
		std::vector<Team*>::iterator itr;
		for(itr = teams.begin(); itr<teams.end(); ++itr)
		{
			Team* team = *itr;
			if(team->getTeamSize() < lowestNumOfPlayers)
			{
				lowestTeam = team;
				lowestNumOfPlayers = team->getTeamSize();
			}
		}
	}

	return lowestTeam;
}