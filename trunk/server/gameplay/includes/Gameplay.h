/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include <math.h>
#include <string>
#include "Team.h"

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Gameplay
{
public:
	Gameplay();
	void			setNumberOfTeams(int num);
	Ogre::Real		getScorePercentage(std::string identifier);
	int				getScoreValue(std::string identifier);
	Team*			createTeam(std::string teamName);
private:
	int				numberOfTeams;
	std::vector<Team*> teams;
};


#endif