/**
* @file        Gameplay.h
* @brief    Manages the view ports for a window
*/
#ifndef TEAM_H
#define TEAM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

#include <math.h>
#include <string>

/*-------------------- FUNCTION DEFINITIONS --------------------*/
class Team
{
public:
    /// @brief  Constructor.
    /// @param  teamNumber   The number which uniquely identifies this team. Should be >= 1
    ///                     (0 is reserved for no-team or free-for-all).
    Team(TeamID id) : mTeamID(id), mVIPPlayer(NULL) {}

    /// @brief  Deconstructor.
    ~Team() {}

    /// @brief  Adds a single player to the team.
    /// @param  player  The player to add.
    void addPlayer(Player* player);

    /// @brief  Returns the list of players on the team.
    /// @return The list of players on the team.
    std::vector<Player*> getPlayers() { return mPlayers; }

    /// @brief  Gets a random player.
    /// @return A random player.
    Player* getRandomPlayer();

    /// @brief  Gets the team's current VIP player.
    /// @return The VIP.
    Player* getVIP() { return mVIPPlayer; }

    /// @brief  Sets the team's VIP player (unsetting the old one if necessary).
    /// @param  player  The new VIP player.
    void setVIP(Player* player);

    /// @brief  Gets the current team size.
    /// @param  The team size.
    int getTeamSize() { return mPlayers.size(); }

    /// @brief  Gets the sum of all team player's health.
    /// @return The total team HP.
    int getTotalTeamHP();

private:
    std::vector<Player*>    mPlayers;
    TeamID                  mTeamID;
    Player*                 mVIPPlayer;
};


#endif