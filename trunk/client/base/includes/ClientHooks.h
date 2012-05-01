/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef CLIENTHOOKS_H
#define CLIENTHOOKS_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "CarCam.h"
#include "Team.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class ClientHooks
{
public:
    static void localPlayerNowInArena(Player *localPlayer);
    static void nonLocalPlayerNowInArena(Player *player);
};

#endif // #ifndef CLIENTHOOKS_H
