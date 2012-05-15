/**
 * @file	ClientHooks.cpp
 * @brief 	The class which we have long needed to separate just utility
            function calls from actual function calls which should be hooked into.
            These are guaranteed to be client only
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "ClientHooks.h"
#include "Player.h"
#include "GameCore.h"

void ClientHooks::localPlayerNowInArena(Player *localPlayer)
{
    static bool hadFirstRunAlready = false;

    if (!hadFirstRunAlready) GameCore::mAudioCore->menuToRockTrack(false);
    localPlayer->getCar()->startEngineSound();
    GameCore::mGui->updateLocalPlayerRank();

    hadFirstRunAlready = true;
}

void ClientHooks::nonLocalPlayerNowInArena(Player *player)
{
    player->getCar()->startEngineSound();
}
