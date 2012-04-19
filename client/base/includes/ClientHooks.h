/**
 * @file	Player.h
 * @brief 	Contains the player car and the related data specific to each player.
 */
#ifndef CLIENTHOOKS_H
#define CLIENTHOOKS_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include "CarCam.h"

enum TeamID;


/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class ClientHooks
{
public:
    static void localPlayerNowInArena(Player *localPlayer);
};

#endif // #ifndef CLIENTHOOKS_H
