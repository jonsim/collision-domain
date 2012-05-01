/**
 * @file	GameGUI.h
 * @brief 	Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef LOBBY_H
#define LOBBY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"

class Lobby
{
public:
    Lobby (CEGUI::Window* guiWindow) : mGUIWindow(guiWindow) {}
    ~Lobby (void) {}

    void setup (const int screenWidth, const int screenHeight);
    void close (void);
    void addServer (std::string serverName, std::string numberOfPlayers, std::string serverMap, bool selected);

private:
    bool enterIPPressed (const CEGUI::EventArgs &args);
    bool refreshPressed (const CEGUI::EventArgs &args);
    bool connectPressed (const CEGUI::EventArgs &args);
    bool IP_connectPressed (const CEGUI::EventArgs &args);
    bool IP_closePressed   (const CEGUI::EventArgs &args);

    CEGUI::Window* mGUIWindow;
};

#endif
