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
public:
	GameGUI();
	~GameGUI();

	void initialiseGUI();

	void setupConnectBox();
    void closeConnectBox();
	bool Connect_Host( const CEGUI::EventArgs &args );
	bool Connect_Quit( const CEGUI::EventArgs &args );

	void setupChatbox();
	void toggleChatbox();
	bool chatboxVisible() { return CEGUI::WindowManager::getSingleton().getWindow( "/Chatbox/input" )->isVisible(); }
	bool Chatbox_Send( const CEGUI::EventArgs &args );
    void chatboxAddMessage( const char *szNickname, char *szMessage );

	void setupConsole();
	void toggleConsole();
	bool consoleVisible() { return CEGUI::WindowManager::getSingleton().getWindow( "/Console" )->isVisible(); }
	bool Console_Send( const CEGUI::EventArgs &args );
	bool Console_Off( const CEGUI::EventArgs &args );

    void setupSpeedo();
    void updateSpeedo();
	void updateSpeedo( float fSpeed, int iGear );

    void setupGearDisplay();

    void updateCounters();
    
private:
	CEGUI::Window *mSheet;

	Ogre::OverlayContainer *olcSpeedo;
	Ogre::OverlayElement   *oleNeedle;
    Ogre::OverlayElement   *oleGear;
};

#endif
