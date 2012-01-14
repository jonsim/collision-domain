/**
 * @file	Player.h
 * @brief 	Contains the Player nodes and the related data.
 */
#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"

/*-------------------- CLASS DEFINITIONS --------------------*/
/// CarType represents the type of car used as the player object (i.e. the model).
enum CarType {SMALL, MEDIUM, LARGE};

/// CarSkin represents the texture applied to the player object.
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class LocalPlayer : public Player
{
 
public:
    void loltest();
};

#endif // #ifndef PLAYER_H
