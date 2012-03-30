/**
* @file		ScoreBoard.h
* @brief	Manages the view ports for a window
*/
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"


/*-------------------- FUNCTION DEFINITIONS --------------------*/
class ScoreBoard
{
public:
			ScoreBoard();
	void	initialize();
private:
	void			displayScreen();
	std::string		buildScoreText();
	Ogre::Overlay *sbOverlay;
	Ogre::OverlayContainer *sbContainer;
	
};


#endif