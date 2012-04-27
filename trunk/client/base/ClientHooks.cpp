/**
 * @file	ClientHooks.cpp
 * @brief 	The class which we have long needed to separate just utility
            function calls from actual function calls which should be hooked into.
            These are guaranteed to be client only
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

void ClientHooks::localPlayerNowInArena(Player *localPlayer)
{
    GameCore::mAudioCore->localPlayerNowInArenaTrigger();
    localPlayer->getCar()->startEngineSound();
}

void ClientHooks::nonLocalPlayerNowInArena(Player *player)
{
    player->getCar()->startEngineSound();
}