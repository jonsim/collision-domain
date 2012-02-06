/**
 * @file	PlayerCollisions.cpp
 * @brief 	Manages collisions (which gradually build up each substep).
            Hopefully this class will be clever enough to trigger only one crash sound per
            "group" of cars involved in each crash, and also not repeat crash noises over
            seperate frames. It will also dispatch noteable crashes to the big screen.
 */
#include "stdafx.h"
#include "SharedIncludes.h"


/// @brief  Constructor to create physics stuff
/// @param  sceneMgr  The Ogre SceneManager which nodes can be attached to.
PlayerCollisions::PlayerCollisions()
{

}


/// @brief  Destructor to clean up
PlayerCollisions::~PlayerCollisions()
{

}


void PlayerCollisions::addCollision(Player* p1, Player* p2, btPersistentManifold* contactManifold)
{
    // the sound will only be played per group of collisions i.e. p1+p2+p4+p99 etc.
    //GameCore::mAudioCore->playCarCrash();

    // Very very crude damage analysis doing no damage to the faster player in the collision
    bool damageP2 = p1->getCar()->getCarMph() > p2->getCar()->getCarMph() ? true : false;
    p1->collisionTickCallback(damageP2 ? 0 : 1);
    p2->collisionTickCallback(damageP2 ? 1 : 0);
    
	/*int numContacts = contactManifold->getNumContacts();
	for (int j = 0; j < numContacts; j++)
	{
		btManifoldPoint& pt = contactManifold->getContactPoint(j);
		if (pt.getDistance() < 0.f)
		{
			const btVector3& ptA = pt.getPositionWorldOnA();
			const btVector3& ptB = pt.getPositionWorldOnB();
			const btVector3& normalOnB = pt.m_normalWorldOnB;
		}
	}*/
}


void PlayerCollisions::frameEventStart()
{
    // clear lists from previous frame

}


void PlayerCollisions::frameEventEnd()
{
    // apply damage and dispatch collision sounds

    // DISPATCH TO BIG SCREEN (IF SERVER)
}
