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


/// 
void PlayerCollisions::addCollision(Player* p1, Player* p2, btPersistentManifold* contactManifold)
{
    // New damage calculations
    Ogre::Real p1MPH = p1->getCar()->getCarMph();
    Ogre::Real p2MPH = p2->getCar()->getCarMph();
    Ogre::Real damage  = abs(p1MPH - p2MPH) * 0.25;  // quarter the difference in speeds
    if (p1MPH > 15 || p2MPH > 15)
    {
        if (p1MPH > p2MPH)
        {
            p1->collisionTickCallback(damage * 0.4);
            p2->collisionTickCallback(damage);
        }
        else
        {
            p1->collisionTickCallback(damage);
            p2->collisionTickCallback(damage * 0.4);
        }
    }
    
    // NOTE WE MUST NOT REMEMBER THE CONTACT MANIFOLD PAST THIS FUNCTION
    for (int i=0; i < contactManifold->getNumContacts(); i++)
    {
        const btManifoldPoint &pt = contactManifold->getContactPoint(i);

        btScalar combinedLateralFriction = pt.m_combinedFriction;
        btScalar appliedImpulse = pt.getAppliedImpulse();
        btScalar distance = pt.getDistance();
        btScalar lifetime = pt.getLifeTime();
        btScalar lateralImpulse1 = pt.m_appliedImpulseLateral1;
        btScalar lateralImpulse2 = pt.m_appliedImpulseLateral2;
        
        btVector3 localA = pt.m_localPointA;
        btVector3 localB = pt.m_localPointB;
        btVector3 normalOnB = pt.m_normalWorldOnB;
        // for deformation

        p1->getCar()->GetHeading();

        btVector3 worldPosOnA = pt.getPositionWorldOnA();
        btVector3 worldPosOnB = pt.getPositionWorldOnB();
        // for sparks

        //btVector3 contactCenter = (worldPosOnA + worldPosOnB) / 2.0;
        /*std::stringstream ss;
        ss << p1 << p2 << "   " << i << "   " <<
            combinedLateralFriction << "   " <<
            appliedImpulse << "   " <<
            distance << "   " <<
            lifetime 
            << "   x" << worldPosOnA.x()
            << " y" << worldPosOnA.y()
            << " z" << worldPosOnA.z()

            << "   x" << worldPosOnB.x()
            << " y" << worldPosOnB.y()
            << " x" << worldPosOnB.z()

            << "   " << lateralImpulse1
            << "   " << lateralImpulse2

            << "   x" << localA.x()
            << " y" << localA.y()
            << " z" << localA.z()

            << "   x" << localB.x()
            << " y" << localB.y()
            << " z" << localB.z()

            << "   x" << normalOnB.x()
            << " y" << normalOnB.y()
            << " z" << normalOnB.z()
            << "\n";
        OutputDebugString(ss.str().c_str());*/
    }



    // contactManifold->getContactBreakingThreshold();
    // contactManifold->getContactPoint();
    // contactManifold->validContactDistance();
    
    // check if player pointer is already somewhere in the lists
    std::list<Player*>* player1AlreadySeen = NULL;
    std::list<Player*>* player2AlreadySeen = NULL;
    std::list<Player*>* emptyList = NULL;
    hasAlreadyBeenSeenThisFrame(&player1AlreadySeen, &player2AlreadySeen, &emptyList, p1, p2);

    if (player1AlreadySeen && player2AlreadySeen)
    {
        // we may have seen these guys in an earlier substep

        if (player1AlreadySeen != player2AlreadySeen)
        {
            // this used to be a never happen case ;) but it actually means there were separate groups
            // of colliding cars and another car has come and bridged these groups to form 1 big collision
            // merge the offending lists onto 1 node

            // merge player2AlreadySeen into player1AlreadySeen
            mergeListsIntoFrom(player1AlreadySeen, player2AlreadySeen, p1, p2);


            player2AlreadySeen->clear();
        }

    }
    else if (player1AlreadySeen || player2AlreadySeen)
    {
        Player* playerToAdd = player1AlreadySeen ? p2 : p1;
        std::list<Player*>* alreadySeen = player1AlreadySeen ? player1AlreadySeen : player2AlreadySeen;

        // append playerToAdd to the cluster of players (which already exists) involved in this specific collision
        alreadySeen->insert(alreadySeen->end(), playerToAdd);

    }
    else
    {
        // neither player has been seen yet so add them as a new collision group
        if (!emptyList)
        {
            // all lists are in use, so lets make another for this collision group
            emptyList = new std::list<Player*>;
            mCollisions->insert(mCollisions->end(), emptyList);
        }

        emptyList->insert(emptyList->end(), p1);
        emptyList->insert(emptyList->end(), p2);
    }

        
    // append the damage dealt in this substep to what we have already

	int numContacts = contactManifold->getNumContacts();
	for (int j = 0; j < numContacts; j++)
	{
		btManifoldPoint& pt = contactManifold->getContactPoint(j);
		if (pt.getDistance() < 0.f)
		{
			const btVector3& ptA = pt.getPositionWorldOnA();
			const btVector3& ptB = pt.getPositionWorldOnB();
			const btVector3& normalOnB = pt.m_normalWorldOnB;
		}
	}
}


void PlayerCollisions::frameEventEnd()
{
    // apply damage and dispatch collision sounds
    {
        //// the sound will only be played per group of collisions i.e. p1+p2+p4+p99 etc.
        int numCollisionGroups = getNumCollisionGroups();
        for (int i=0; i < numCollisionGroups; i++)
        {
            //OgreOggISound* sound = GameCore::mAudioCore->getSoundInstance(CAR_CRASH, GameCore::mPhysicsCore->getUniqueEntityID());
            //GameCore::mAudioCore->playSoundOrRestart(sound);
            //GameCore::mAudioCore->deleteSoundInstance(sound);
        }
        ////GameCore::mAudioCore->playCarCrash();

        //// Very very crude damage analysis doing no damage to the faster player in the collision
        //bool damageP2 = p1->getCar()->getCarMph() > p2->getCar()->getCarMph() ? true : false;
        //p1->collisionTickCallback(damageP2 ? 0 : 1);
        //p2->collisionTickCallback(damageP2 ? 1 : 0);
    }

    // DISPATCH TO BIG SCREEN (IF SERVER)
    {

    }

    // clear lists from previous frame
    //printLists();

    emptyLists();

}


void PlayerCollisions::printLists()
{
    OutputDebugString("___________________________\n");

    std::list<std::list<Player*>*>::iterator i = mCollisions->begin();
    while (i != mCollisions->end())
    {
        std::list<Player*>* subList = *i;

        std::string size = boost::lexical_cast<std::string>(subList->size());
        std::string output = "Size " + size + "\n";
        
        OutputDebugString(output.c_str());

        std::list<Player*>::iterator jj = subList->begin();
        while (jj != subList->end())
        {
            Player* element = *jj;

            std::string address = boost::lexical_cast<std::string>(element);
            std::string output = "      " + address + "\n";
        
            OutputDebugString(output.c_str());

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