/**
 * @file	GameGUI.h
 * @brief 	Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef LOBBY_H
#define LOBBY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

class Lobby
{
public:
	Lobby();
	~Lobby();

    void setupLobby (const int screenWidth, const int screenHeight);
    void closeLobby (void);
    void lobbyAddServer (const char* serverName, const char* numberOfPlayers, const char* serverMap, bool selected);
    bool lobbyEnterIPPressed (const CEGUI::EventArgs &args);
    bool lobbyRefreshPressed (const CEGUI::EventArgs &args);
    bool lobbyConnectPressed (const CEGUI::EventArgs &args);
    bool lobbyIP_connectPressed (const CEGUI::EventArgs &args);
    bool lobbyIP_closePressed   (const CEGUI::EventArgs &args);

private:
	CEGUI::Window *mSheet;
};

#endif
