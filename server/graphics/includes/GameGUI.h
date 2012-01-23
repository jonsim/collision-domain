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
	CEGUI::Window *mSheet;

public:

	GameGUI();
	~GameGUI();

	void setupGUI();

	void displayChatbox();
	void toggleChatbox();
	bool chatboxVisible() { return CEGUI::WindowManager::getSingleton().getWindow( "/Chatbox/input" )->isVisible(); }
	bool Chatbox_Send( const CEGUI::EventArgs &args );

	void displayConsole();
	void toggleConsole();
	bool consoleVisible() { return CEGUI::WindowManager::getSingleton().getWindow( "/Console" )->isVisible(); }
	bool Console_Send( const CEGUI::EventArgs &args );
	bool Console_Off( const CEGUI::EventArgs &args );
};

#endif
