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
	void	hide();
	void	update();
private:
	void			displayScreen();
	std::string		buildScoreText();
	Ogre::Overlay *sbOverlay;
	Ogre::OverlayContainer *sbContainer;
	bool isInitialized;
	bool isShown;

	Ogre::OverlayElement *textAreaT1;
	Ogre::OverlayElement *textAreaT2;
};


#endif