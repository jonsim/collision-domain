/**
* @file		HUD.h
* @brief	Manages the view ports for a window
*/
#ifndef HUD_H
#define HUD_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class HUD
{
public:
			HUD();
	void	HUD::initialize();
	void	HUD::drawDeathMessage(Player* killer, Player* killed);
	
private:
	void HUD::setupWOD(); // Setup wall of DEATH

	Ogre::Overlay*			wodOverlay;
	Ogre::OverlayContainer* wodContainer;
	Ogre::OverlayElement*   wodTextArea;
};


#endif