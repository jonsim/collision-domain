/**
 * @file	PlayerCollisions.cpp
 * @brief 	Manages collisions (which gradually build up each substep).
            Hopefully this class will be clever enough to trigger only one crash sound per
            "group" of cars involved in each crash, and also not repeat crash noises over
            seperate frames. It will also dispatch noteable crashes to the big screen.
 */
#include "stdafx.h"
#include "PlayerCollisions.h"
#include "boost/lexical_cast.hpp"

#define MAX_DAMAGE 400
#define BIG_CRASH_THRESHOLD 80

/// @brief  Constructor to create physics stuff
/// @param  sceneMgr  The Ogre SceneManager which nodes can be attached to.
PlayerCollisions::PlayerCollisions()
{
    // any given Player pointer can only appear on one of the linkedlist nodes over the total list
    // eventually alongside Player*, collision information such as force and locations will be stored
    mCollisions = new std::list< std::list<Player*>* >;
}


/// @brief  Destructor to clean up
PlayerCollisions::~PlayerCollisions()
{
    std::list<std::list<Player*>*>::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*>* subList = *i;
        delete subList;
        ++i;
    }

    delete mCollisions;
}

static int numColls = 0;

static float massPairs[3][3] = {
        { 1.0f, 0.8f, 1.2f },
        { 1.2f, 1.0f, 1.4f },
        { 1.0f, 0.8f, 1.0f }
    };

void PlayerCollisions::addCollision(Player* p1, Player* p2, btPersistentManifold* contactManifold) {
	/*
		    // SET THESE:
		    btVector3 crashLocation(,,);
		    Car *eitherOfTheTwoCarObjectsItMakesNoDifference = ;
		    float crashIntensity = 0.8;

		    // Call with the location of the crash and the intensity between 0 and 1, ideally between 0 and 0.8
		    eitherOfTheTwoCarObjectsItMakesNoDifference->triggerCrashSoundAt(
		        BtOgre::Convert::toOgre(crashLocation),
		        crashIntensity);
	*/

    // **NOTE** WE MUST NOT REMEMBER TO READ THE FOLLOWING COMMENT
    
    // NOTE WE MUST NOT REMEMBER THE CONTACT MANIFOLD PAST THIS FUNCTION

    if( p1 == NULL || p2 == NULL )
        return;

	// number of contact points usually > 1, so average all of the relevant values
    btVector3 averageCollisionPointOnA(0, 0, 0);
	btVector3 averageCollisionPointOnB(0, 0, 0);
	//btVector3 averageNormOnB(0, 0, 0);

	btScalar averageOverlapDistance = 0.f;
	int numContacts = contactManifold->getNumContacts();

	Ogre::Real p1MPH = abs(p1->getCar()->getCarMph());
    Ogre::Real p2MPH = abs(p2->getCar()->getCarMph());

	for (int j = 0; j < numContacts; j++) {
		btManifoldPoint& pt = contactManifold->getContactPoint(j);
		if (pt.getDistance() < 0.f) {
			averageCollisionPointOnA += pt.getPositionWorldOnA();
			averageCollisionPointOnB += pt.getPositionWorldOnB();
			averageOverlapDistance += pt.getDistance();
			//averageNormOnB +=  pt.m_normalWorldOnB;
		}
	}

	// some collisions dont have contact points (dickheads), ignore them
	if(numContacts > 0) {
		averageCollisionPointOnA /= numContacts;
		averageCollisionPointOnB /= numContacts;
		averageOverlapDistance /= numContacts;
		// dont want to be thinking about damage if neither car is going morethan 15mph, or if there is no penetration
		if ((p1MPH > 15 || p2MPH > 15) && averageOverlapDistance < 0) {
			// there is a collision
			/* 
			     collisionDelays, keeps a counter of the number of frames since each player has been in a collision
				 Cars are added to collisionDelays on first crash
			*/
			if(collisionDelays[p1] == NULL) {
				collisionDelays[p1] = 100;
			}
            
			if(collisionDelays[p2] == NULL) {
				collisionDelays[p2] = 100;
			}
			// if either player hasn't collided for more than 15 frames, let them collide again
			if(collisionDelays[p1] > 90 || collisionDelays[p2] > 90) {
				// reset frame counter
                if(p1 == NULL || p2 == NULL) return;
				collisionDelays[p1] = collisionDelays[p2] = 0;
                int crashType;
                
                Ogre::Vector3 localOnA = p1->getCar()->mBodyNode->convertWorldToLocalPosition((Ogre::Vector3)averageCollisionPointOnA);
                Ogre::Vector3 localOnB = p2->getCar()->mBodyNode->convertWorldToLocalPosition((Ogre::Vector3)averageCollisionPointOnB);
                Ogre::Real combinedSpeed = p1MPH + p2MPH;
                Ogre::Real damageShareToA = p1MPH / combinedSpeed;
                Ogre::Real damageShareToB = p2MPH / combinedSpeed;

                Ogre::Real totalDamage = abs(averageOverlapDistance * 20000.f);
                totalDamage = totalDamage > MAX_DAMAGE ? (float)MAX_DAMAGE : totalDamage;

                Ogre::Real damageToA = totalDamage * damageShareToB;
                Ogre::Real damageToB = totalDamage * damageShareToA;

                int sectionOnA = getSectionOnCar(p1, localOnA);
                int sectionOnB = getSectionOnCar(p2, localOnB);

                int sectionTestA = sectionOnA < 2 ? 0 : sectionOnA < 4 && sectionOnA >=2 ? 1 : 2; 
                int sectionTestB = sectionOnB < 2 ? 0 : sectionOnB < 4 && sectionOnB >=2 ? 1 : 2; 

                damageToA *= massPairs[p1->getCarType()][p2->getCarType()];
                damageToB *= massPairs[p2->getCarType()][p1->getCarType()];

                if(sectionTestA < sectionTestB) {
                    damageToA *= 0.8f;
                    damageToB *= 1.2f;
                } else if(sectionTestB < sectionTestA) {
                    damageToB *= 0.8f;
                    damageToA *= 1.2f;
                }

                if(totalDamage < BIG_CRASH_THRESHOLD && (p1MPH > 40 || p2MPH > 40)) {
		            crashType = 1;
	            } else if(totalDamage < BIG_CRASH_THRESHOLD && (p1MPH < 40 && p2MPH < 40)) {
		            crashType = 2;
	            } else if(totalDamage >= BIG_CRASH_THRESHOLD) {
                    crashType = 3;
	            }

				p1->collisionTickCallback((Ogre::Vector3)averageCollisionPointOnA, damageToA, sectionOnA, crashType, p2);
				p2->collisionTickCallback((Ogre::Vector3)averageCollisionPointOnB, damageToB, sectionOnB, crashType, p1);


			}
		}

	} 
}

int PlayerCollisions::getSectionOnCar(Player *p, Ogre::Vector3 pos) {
    // FL,FR,ML,MR,RL,RR = 0,1,2,3,4,5
    int r;
    if(pos.x <= 0.f) { // RHS
        if(pos.z > p->frontDamageBoundary) {
            // FR
            r = 1;
        } else if(pos.z > p->rearDamageBoundary && pos.z <= p->frontDamageBoundary) {
            // MR
            r = 3;
        } else if(pos.z <= p->rearDamageBoundary) {
            // RR
            r = 5;
        }
    } else {
        if(pos.z > p->frontDamageBoundary) {
            // FL
            r = 0;
        } else if(pos.z > p->rearDamageBoundary && pos.z <= p->frontDamageBoundary) {
            // ML
            r = 2;
        } else if(pos.z <= p->rearDamageBoundary) {
            // RL
            r = 4;
        }
    }
    return r;
}


void PlayerCollisions::frameEventEnd()
{
	collisionDelaysItr = collisionDelays.begin();
	for(;collisionDelaysItr != collisionDelays.end(); collisionDelaysItr++) {
		if(collisionDelaysItr->second < 100) {
			collisionDelaysItr->second += 1;
		}
	}
    
    // apply damage and dispatch collision sounds
    //// the sound will only be played per group of collisions i.e. p1+p2+p4+p99 etc.
    /*int numCollisionGroups = getNumCollisionGroups();
    for (int i=0; i < numCollisionGroups; i++)
    {
        //OgreOggISound* sound = GameCore::mAudioCore->getSoundInstance(CAR_CRASH, GameCore::mPhysicsCore->getUniqueEntityID());
        //GameCore::mAudioCore->playSoundOrRestart(sound);
        //GameCore::mAudioCore->deleteSoundInstance(sound);
    }*/
    ////GameCore::mAudioCore->playCarCrash();
    
    // clear lists from previous frame
    //emptyLists();

}


void PlayerCollisions::printLists()
{
    //OutputDebugString("___________________________\n");

    std::list<std::list<Player*>*>::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*>* subList = *i;

        std::string size = boost::lexical_cast<std::string>(subList->size());
        std::string output = "Size " + size + "\n";
        
        //OutputDebugString(output.c_str());

        std::list<Player*>::iterator jj = subList->begin();
        while (jj != subList->end())
        {
            Player* element = *jj;

            std::string address = boost::lexical_cast<std::string>(element);
            std::string output = "      " + address + "\n";
        
            //OutputDebugString(output.c_str());

            ++jj;
        }

        ++i;
    }
}


/// Does not clear the main list (of lists), only sublists, leaving the
/// main list either empty or full with empty sublists.
void PlayerCollisions::emptyLists()
{
    std::list<std::list<Player*>*>::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*>* subList = *i;
        subList->clear(); // sublist can never be null
        ++i;
    }
}


/// If both player1AlreadySeen and player2AlreadySeen are set to null by this method,
/// it will attempt to also find an emptyList (so we don't have to keep making new ones)
void PlayerCollisions::hasAlreadyBeenSeenThisFrame(
    std::list<Player*> **player1AlreadySeen,
    std::list<Player*> **player2AlreadySeen,
    std::list<Player*> **emptyList,
    Player *p1,
    Player *p2)
{
    *player1AlreadySeen = NULL;
    *player2AlreadySeen = NULL;
    *emptyList = NULL;

    std::list<std::list<Player*>*>::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*>* subList = *i;
        //--------------------

        // sublist can never be null
        if (!*emptyList && subList->size() == 0)
        {
            *emptyList = subList;
        }
        
        // lets find the player pointer if they are here
        std::list<Player*>::iterator jj = subList->begin();
        while (jj != subList->end())
        {
            Player* element = *jj;
            //--------------------
            if (element == p1)
            {
                *player1AlreadySeen = subList;
                if (*player2AlreadySeen) return;
            }

            if (element == p2)
            {
                *player2AlreadySeen = subList;
                if (*player1AlreadySeen) return;
            }

            //--------------------
            ++jj;
        }

        //--------------------
        ++i;
    }
}


void PlayerCollisions::mergeListsIntoFrom(
    std::list<Player*> *intoListContainingP1,
    std::list<Player*> *fromListContainingP2,
    Player *p1,
    Player *p2)
{
    std::list<Player*>::iterator i = fromListContainingP2->begin();
    while (i != fromListContainingP2->end())
    {
        Player *element = *i;

        // copy all the elements from this node into the target one
        intoListContainingP1->insert(intoListContainingP1->end(), element);

        ++i;
    }
}

int PlayerCollisions::getNumCollisionGroups()
{
    int count = 0;

    std::list< std::list<Player*>* >::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*> *subList = *i;

        if (subList->size() > 0) count++;

        ++i;
    }

    return count;
}
