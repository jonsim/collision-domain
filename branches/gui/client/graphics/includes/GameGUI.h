/**
 * @file	GameGUI.h
 * @brief 	Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef GAMEGUI_H
#define GAMEGUI_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"

class GameGUI
{
private:
	CEGUI::OgreRenderer *mGuiRenderer;
	CEGUI::Window *mSheet;

public:

	GameGUI( CEGUI::OgreRenderer *renderer );
	~GameGUI();

	void setupGUI();
	void displayConnectBox();

	bool Connect_Host( const CEGUI::EventArgs &args );
};

#endif
