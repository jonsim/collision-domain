/**
* @file        Gameplay.h
* @brief    Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "InfoItem.h"
#include "ScoreBoard.h"
#include "HUD.h"
#include "Death.h"

#include <math.h>
#include <string>
#include "RakNetTypes.h"

/*-------------------- Some Game Values -------------*/
#define NUM_PLAYERS_TO_START 5
#define NUM_TOP_PLAYERS 5

/*-------------------- ENUM TYPES -------------------*/
enum
{
    FREE_FOR_ALL_MODE,
    TEAM_FREE_FOR_ALL_MODE,
    KING_OF_THE_HILL_MODE,
    VIP_MODE
};

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Gameplay
{
public:
                Gameplay() : mGameActive(false), mNumberOfTeams(0) {}
                ~Gameplay() {}
    Team*       createTeam(int teamNumber);
    void        createTeams(int nummberOfTeams);
    float       getScorePercentage(int teamNumber);
    int         getScoreValue(int teamNuber);
    //void        gameOver();
    //bool        hasWon(Team* team);
    //Team*       checkIfGameOver();
    void        setGameplayMode(int gameplayMode) { mGameplayMode = gameplayMode; }
    void        setNewVIP(Team* team);
    void        setNewVIPs();
    Team*       addPlayer( RakNet::RakNetGUID playerid, int requestedTeamNumber ); //Returns true if the player has been added to big screen
    void                        notifyDamage(Player* player);
    void                        preparePlayers(); //Place the palyers in the correct place
    void                        resetAllHP();
    void                        positionPlayers();
    void                        startGame();
    void                        drawInfo(); //Draws any info that we require
    void                        setupOverlay();
    Team*                       getTeam(int i);
    void                        drawDeathInfo();
    void                        initialize();

    std::vector<InfoItem*>      mInfoItems;
    int                         mNumberOfTeams;
    bool                        mGameActive; //True = Game underway, False = Game not yet started
    void                        markDeath(Player* deadPlayer, Player* causedBy);
    std::vector<DEATH*>         getDeathList();
    void                        restartGame();

private:
    //Methods
    void                        vipModeGameWon();
    Team*                       autoAssignTeam();
    bool                        validateTeamChoice(int requestedTeamNumber);
    void                        printTeamStats();
    void                        scheduleCountDown();
    //Variabels
    std::vector<Team*>            teams;
    int                            mGameplayMode;
    
    Ogre::OverlayContainer* olContainer;
    void                        handleInfoItem(InfoItem* item, bool show);

    ScoreBoard*                    mSB;
    HUD*                        mHUD;
    std::vector<DEATH*>            deathList;
    void                        calculateRoundScores();
    Player*                        topPlayers[NUM_TOP_PLAYERS];
};


#endif