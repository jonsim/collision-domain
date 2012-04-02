/**
* @file        Gameplay.h
* @brief    Manages the view ports for a window
*/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"
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
#define NUM_TEAMS 2

/*-------------------- ENUM TYPES -------------------*/
// Represents a game mode. FFA = Free for all; TDM = Team deathmatch, KOTH = King of the hill.
enum Gamemode {FFA_MODE, TDM_MODE, KOTH_MODE, VIP_MODE};

// Represents the teams, as determined by NUM_TEAMS.
#if NUM_TEAMS == 2
    enum TeamID  {NO_TEAM = 0, BLUE_TEAM = 1, RED_TEAM = 2};
#else
    #error "TeamID not set up for !2 teams."
#endif


/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Team;


class Gameplay
{
public:
    /// @brief  Constructor.
    Gameplay() : mGameActive(false), mGamemode(VIP_MODE) { createTeams(); }

    /// @brief  Destructor.
    ~Gameplay() {}

    /// @brief  Creates the default number of teams (NUM_TEAMS). Limits on this number exist in the textures 
    ///         available etc. so it will only be changed following a large rework.
    void  createTeams();

    /// @brief  Gets a team pointer corresponding to the TeamID given.
    /// @param  id  The TeamID of the team to retrieve.
    /// @param  return  A pointer to the team given by the TeamID.
    Team* getTeam(TeamID id) { return (id == NO_TEAM) ? NULL : teams[id-1]; }

    /// @brief  Adds a player to the team requested (the request should originate from the client).
    /// @param  playerid    The RakNetGUID for the player.
    /// @param  requestedTeam   The TeamID of the team the client has requested to join. This can be NO_TEAM if the
    ///                         player does not have a preference and wishes to be automatically assigned a team.
    /// @return Indicates whether the request was a success (true) or not (false).
    bool  addPlayer( RakNet::RakNetGUID playerid, TeamID requestedTeam );

    /// @brief  Sets the Gamemode.
    /// @param  mode    The new Gamemode
    void  setGamemode(Gamemode mode) { mGamemode = mode; }

    /// @brief  Starts the entire game.
    void  startGame();

    // VIP Mode functions
    void  setNewVIP(TeamID id);
    void  setNewVIPs();
    //float       getScorePercentage(int teamNumber);
    //int         getScoreValue(int teamNuber);
    //void        gameOver();
    //bool        hasWon(Team* team);
    //Team*       checkIfGameOver();
    void                        notifyDamage(Player* player);
    void                        drawInfo(); //Draws any info that we require
    void                        setupOverlay();
    void                        drawDeathInfo();
    //void                        initialize();

    //std::vector<InfoItem*>      mInfoItems;
    //int                         mNumberOfTeams;
    bool                        mGameActive; //True = Game underway, False = Game not yet started
    //void                        markDeath(Player* deadPlayer, Player* causedBy);
    //std::vector<DEATH*>         getDeathList();
    //void                        restartGame();

private:
    /// @brief  Automatically chooses a team based on which team has the least players.
    /// @return The team with the least players.
    Team* autoAssignTeam();

    /// @brief  Checks whether or not the requested team is available to join (if the player count is sufficiently low).
    /// @return Whether or the team is available (true) or not (false).
    bool  validateTeamChoice(TeamID requestedTeam);

    /// @brief  Starts a round.
    void  startRound();

    /// @brief  Finishes a round.
    void  endRound();

    /// @brief Prepares the players for the start of a round.
    void  preparePlayers();

    /// @brief  Resets all players' health pools.
    void  resetAllHP();

    /// @brief  Positions the players in a circle.
    void  positionPlayers();

    //void  vipModeGameWon();
    void  printTeamStats();
    void  scheduleCountDown();
    //void                        handleInfoItem(InfoItem* item, bool show);

    Team*    teams[NUM_TEAMS];
    Gamemode mGamemode;
    //Ogre::OverlayContainer* olContainer;
    ScoreBoard*                    mSB;
    HUD*                        mHUD;
    std::vector<DEATH*>            deathList;
    void                        calculateRoundScores();
    Player*                        topPlayers[NUM_TOP_PLAYERS];
};


#endif