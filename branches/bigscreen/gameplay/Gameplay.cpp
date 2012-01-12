/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "includes\Gameplay.h"


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
