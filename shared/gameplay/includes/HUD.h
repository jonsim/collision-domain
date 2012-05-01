/**
* @file		HUD.h
* @brief	Manages the view ports for a window
*/
#ifndef HUD_H
#define HUD_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"

class Player;

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class HUD
{
public:
			HUD();
	void	initialize();
	void	drawDeathMessage(Player* killer, Player* killed);
	
private:
	void setupWOD(); // Setup wall of DEATH

	Ogre::Overlay*			wodOverlay;
	Ogre::OverlayContainer* wodContainer;
	Ogre::OverlayElement*   wodTextArea;
};


#endif
