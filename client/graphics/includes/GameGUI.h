/**
 * @file        GameGUI.h
 * @brief       Functions to create and display GUI elements on-screen, and handle call-backs
 */
#ifndef GAMEGUI_H
#define GAMEGUI_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "NetworkCore.h"
#include "Car.h"

#define COLLISION_DOMAIN_CLIENT

class GameGUI
{
public:
    GameGUI (void)
      : oleDamage(NULL),
        olRank(NULL)
    {
    }

    ~GameGUI (void) {}

    void setupSpawnScreen (CEGUI::Window* guiWindow);
    void showSpawnScreenPage1 (GameMode gameMode);
    void showSpawnScreenPage2 (GameMode gameMode, TeamID teamDisplay, CarType defaultCar = CAR_BANGER);
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
    inline bool spawnScreenVisible (void) { return CEGUI::WindowManager::getSingleton().getWindow("/SpawnScreen/Team")->isVisible() || CEGUI::WindowManager::getSingleton().getWindow("/SpawnScreen/Vehicle")->isVisible(); }

    void flipAllVisibility (void);

    void setupChatbox (CEGUI::Window* guiWindow);
    void toggleChatbox (void);
    inline bool chatboxVisible (void)
    {
        CEGUI::Window *window = CEGUI::WindowManager::getSingleton().getWindow("/Chatbox/input");
        // rare crash fix (I don't know why)
        if (window == NULL) return false;
        return window->isVisible();
    }
    bool Chatbox_Send (const CEGUI::EventArgs& args);
    void chatboxAddMessage (const char *szNickname, char *szMessage);

    void setupConsole (CEGUI::Window* guiWindow);
    void toggleConsole (void);
    inline bool consoleVisible (void) { return CEGUI::WindowManager::getSingleton().getWindow("/Console")->isVisible(); }
    bool Console_Send (const CEGUI::EventArgs& args);
    bool Console_Off (const CEGUI::EventArgs& args);
    void hideOverlaysForBigScreen();
    void showOverlaysForBigScreen();
    void toggleRankDisplay(bool setOn);

    void setupOverlays (CEGUI::Window* guiWindow);
    void setupFPSCounter (CEGUI::Window* guiWindow);
    void toggleFPSCounter (void);

    void updateLocalPlayerRank();
    void updateSpeedo (void);
    void updateSpeedo (float fSpeed, int iGear);
    void updateDamage (CarType ct, int part, int colour);

    void updateCounters (void);
    void setupDamageDisplay(CarType carType, TeamID tid);
    void breakGlass(int level);
    
private:
    int currentRankMaterialIndex;
    void setupRank(void);
    void setupSpeedo (void);
    void setupGearDisplay (void);
    void updateRank (int rankIndex, bool isShared);
    int  getUnstretchedWidth(int outputPxWidth);
    
    Ogre::Overlay          *olRank;
    Ogre::OverlayContainer *olRankContainer;

    Ogre::Overlay           *olSpeedo;
    Ogre::OverlayContainer *olcSpeedo;
    Ogre::OverlayContainer *oleDamage;

    Ogre::OverlayContainer *oleGlass;

    Ogre::OverlayElement *glass1;
    Ogre::OverlayElement *glass2;
    Ogre::OverlayElement *glass3;

    Ogre::OverlayElement* oleNeedle;
    Ogre::OverlayElement* oleGear;

    Ogre::OverlayElement* oleDamageEngine;
    Ogre::OverlayElement* oleDamageFL;
    Ogre::OverlayElement* oleDamageFR;
    Ogre::OverlayElement* oleDamageRL;
    Ogre::OverlayElement* oleDamageRR;

	Ogre::OverlayElement   *damageHUD_Body;
	Ogre::OverlayElement   *damageHUD_TL;
	Ogre::OverlayElement   *damageHUD_TR;
	Ogre::OverlayElement   *damageHUD_BL;
	Ogre::OverlayElement   *damageHUD_BR;
	Ogre::OverlayElement   *damageHUD_ML;
	Ogre::OverlayElement   *damageHUD_MR;

    Ogre::Overlay   *glassOverlay;

    GameMode currentMode;
    TeamID   playerTeam;

    CarType playerCarType; //

    //Ogre::OverlayElement   *oleDamageEngine, *oleDamageFL, *oleDamageFR, *oleDamageRL, *oleDamageRR;
    CEGUI::Window*   spawnScreenImage;
    CEGUI::Imageset* spawnScreenImageSet;
    TeamID           spawnScreenTeamSelection;
    CarType          spawnScreenCarSelection;
};

#endif
