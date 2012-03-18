/**
* @file		Gameplay.h
* @brief	Manages the view ports for a window
*/
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "Death.h"

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class GameManager
{
public:
								GameManager();
	void						fetchCurrentGameInfo();
private:
	std::vector<std::vector<DEATH*>>	deathLists;
};


#endif