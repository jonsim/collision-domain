/**
 * @file	PlayerCollisions.cpp
 * @brief 	Manages collisions (which gradually build up each substep).
            Hopefully this class will be clever enough to trigger only one crash sound per
            "group" of cars involved in each crash, and also not repeat crash noises over
            seperate frames. It will also dispatch noteable crashes to the big screen.
 */
#ifndef PLAYERCOLLISIONS_H
#define PLAYERCOLLISIONS_H

#include "stdafx.h"
#include "SharedIncludes.h"

class PlayerCollisions
{
public:
    PlayerCollisions();
    virtual ~PlayerCollisions();
    void addCollision(Player* p1, Player* p2, btPersistentManifold* contactManifold);
    void frameEventStart();
    void frameEventEnd();

private:
};

#endif // #ifndef PLAYERCOLLISIONS_H

