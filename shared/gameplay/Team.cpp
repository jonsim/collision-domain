/**
* @file        Team.cpp
* @brief    Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

void Team::addPlayer(Player *player)
{
    // Update the player's team (and graphics if neceesary) and add to the player list.
    player->setTeam(mTeamID);
    mPlayers.push_back(player);
}

Player* Team::getRandomPlayer()
{
    if(mPlayers.size() <= 0)
        return NULL;
    
    unsigned int pickNum = rand() % mPlayers.size();
    return mPlayers[pickNum];
}

void Team::setVIP(Player* player)
{
    // Check the player passed is valid (could it not be?)
    if (player == NULL)
        return;

    OutputDebugString("Setting a new VIP player\n");

    // Unset the old VIP (if one exists)
    if (mVIPPlayer != NULL)
        mVIPPlayer->setVIP(false);

    // Set the new VIP
    mVIPPlayer = player;
    mVIPPlayer->setVIP(true);

    // Notify the client
    #ifdef COLLISION_DOMAIN_SERVER
        GameCore::mNetworkCore->declareNewVIP(mVIPPlayer);
    #endif
}

int Team::getTotalTeamHP()
{
    int totalHP = 0;
    std::vector<Player*>::iterator itr;
    for(itr = mPlayers.begin(); itr < mPlayers.end(); ++itr)
        totalHP += (*itr)->getHP();
    return totalHP;
}