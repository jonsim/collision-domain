/**
* @file		ScoreBoard.h
* @brief	Manages the view ports for a window
*/
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
#include "Team.h"

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
    std::string		buildScoreText(TeamID teamID);
	Ogre::Overlay *sbOverlay;
	Ogre::OverlayContainer *sbContainer;
    Ogre::Overlay *textScoreOverlay;
	bool isInitialized;
	bool isShown;
	bool isForced;
    std::vector<Ogre::OverlayElement*> redTeamStrips;
    std::vector<Ogre::OverlayElement*> blueTeamStrips;
    std::vector<Ogre::OverlayElement*> greyTeamStrips;

	Ogre::OverlayElement *textAreaT1;
	Ogre::OverlayElement *textAreaT2;
    Ogre::OverlayElement *textAreaT3;

    void    manageStrips();
};


#endif