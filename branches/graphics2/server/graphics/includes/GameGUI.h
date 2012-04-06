/**
 * @file	GameGUI.h
 * @brief 	Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef GAMEGUI_H
#define GAMEGUI_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"
#include <stdio.h>
#include <stdarg.h>

class GameGUI
{
public:

	GameGUI();
	~GameGUI();

	void initialiseGUI();
    
	void setupConsole();
    bool receiveFromConsole( const CEGUI::EventArgs &args );
    void outputToConsole( const char* str, ... );

private:
	CEGUI::Window* mSheet;
};

#endif
