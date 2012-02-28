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
    void frameEventEnd();
    
    void printLists();
private:
    void emptyLists();
    void hasAlreadyBeenSeenThisFrame(
        std::list<Player*> **player1AlreadySeen,
        std::list<Player*> **player2AlreadySeen,
        std::list<Player*> **emptyList,
        Player* p1,
        Player* p2);
    void mergeListsIntoFrom(
        std::list<Player*> *intoListContainingP1,
        std::list<Player*> *fromListContainingP2,
        Player *p1,
        Player *p2);
    int getNumCollisionGroups();

    std::list< std::list<Player*>* > *mCollisions;
};

#endif // #ifndef PLAYERCOLLISIONS_H

