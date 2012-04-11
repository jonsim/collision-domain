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
	void	show();
	void	showForce(); //This shows the scoreboard and forces it to stay
	void	hide();
	void	hideForce(); //This hides a forced scoreboard
	void	update();
private:
	void			displayScreen();
	std::string		buildScoreText();
	Ogre::Overlay *sbOverlay;
	Ogre::OverlayContainer *sbContainer;
	bool isInitialized;
	bool isShown;
	bool isForced;

	Ogre::OverlayElement *textAreaT1;
	Ogre::OverlayElement *textAreaT2;
};


#endif