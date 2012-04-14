/**
 * @file        GameGUI.h
 * @brief       Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef GAMEGUI_H
#define GAMEGUI_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"

class GameGUI
{
public:
    GameGUI (void) {}
    ~GameGUI (void) {}

    void setupSpawnScreen (CEGUI::Window* guiWindow);
    void showSpawnScreenPage2 (void);
    void showSpawnScreenPage1 (void);
    void closeSpawnScreen (void);
    void showSpawnScreenErrorText (const char* errorText);
    void hideSpawnScreenErrorText (void);
    bool SpawnScreen_p1btnBlueTeam (const CEGUI::EventArgs& args);
    bool SpawnScreen_p1btnRedTeam (const CEGUI::EventArgs& args);
    bool SpawnScreen_p1btnAutoAssign (const CEGUI::EventArgs& args);
    bool SpawnScreen_p1btnSpectator (const CEGUI::EventArgs& args);
    bool SpawnScreen_p1btnProjector (const CEGUI::EventArgs& args);
    bool SpawnScreen_p2btnCoupe (const CEGUI::EventArgs& args);
    bool SpawnScreen_p2btnHatchback (const CEGUI::EventArgs& args);
    bool SpawnScreen_p2btnTruck (const CEGUI::EventArgs& args);
    bool SpawnScreen_p2btnConfirm (const CEGUI::EventArgs& args);
    bool SpawnScreen_p2btnCancel (const CEGUI::EventArgs& args);

        void setupChatbox (CEGUI::Window* guiWindow);
        void toggleChatbox (void);
        inline bool chatboxVisible (void) { return CEGUI::WindowManager::getSingleton().getWindow("/Chatbox/input")->isVisible(); }
        bool Chatbox_Send (const CEGUI::EventArgs& args);
    void chatboxAddMessage (const char *szNickname, char *szMessage);

        void setupConsole (CEGUI::Window* guiWindow);
        void toggleConsole (void);
        inline bool consoleVisible (void) { return CEGUI::WindowManager::getSingleton().getWindow("/Console")->isVisible(); }
        bool Console_Send (const CEGUI::EventArgs& args);
        bool Console_Off (const CEGUI::EventArgs& args);

    void setupOverlays (CEGUI::Window* guiWindow);
    void setupFPSCounter (CEGUI::Window* guiWindow);

    void updateSpeedo (void);
        void updateSpeedo (float fSpeed, int iGear);
    void updateDamage (int part, int colour);

    void updateCounters (void);
    
private:
    void setupSpeedo (void);
    void setupGearDisplay (void);
    void setupDamageDisplay (void);

        Ogre::OverlayContainer* olcSpeedo;
    Ogre::OverlayContainer* oleDamage;

        Ogre::OverlayElement* oleNeedle;
    Ogre::OverlayElement* oleGear;

    Ogre::OverlayElement* oleDamageEngine;
    Ogre::OverlayElement* oleDamageFL;
    Ogre::OverlayElement* oleDamageFR;
    Ogre::OverlayElement* oleDamageRL;
    Ogre::OverlayElement* oleDamageRR;

    CEGUI::Window*   spawnScreenImage;
    CEGUI::Imageset* spawnScreenImageSet;
    int              spawnScreenTeamSelection;
};

#endif
