
#include "stdafx.h"
#include "PlayerPool.h"
#include "Player.h"
#include "GameCore.h"

#define BASIC_INTERP 1

PlayerPool::PlayerPool() : mLocalPlayer(0)
{

}

int PlayerPool::addPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
    int compare = strncmp ( szNickname, "AiPlayer", 8 );
    bool isAI = compare == 0 ? true : false;

	int iNew = mPlayers.size();
	mPlayers.push_back(new Player(isAI));
	mPlayers[iNew]->setPlayerGUID(playerid);
	mPlayers[iNew]->setGUID(playerid);
	//mGUID.push_back(playerid);
    mPlayers[iNew]->setNickname( szNickname );

	return iNew;
}

int PlayerPool::getNumberOfPlayers()
{
	return mPlayers.size();
}

void PlayerPool::addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	mLocalPlayer = new Player(false);
	mLocalPlayer->setPlayerGUID(playerid);
	mLocalPlayer->setNickname(szNickname);
	mLocalGUID = playerid;
	
	mPlayers.push_back(mLocalPlayer);
}

bool PlayerPool::delPlayer( RakNet::RakNetGUID playerid )
{
	Player *pPlayer = getPlayer( playerid );
	if( pPlayer )
	{
        GameCore::mGameplay->playerQuit( pPlayer );
        std::vector<Player*>::iterator it = find( mPlayers.begin(), mPlayers.end(), pPlayer );
        if( it != mPlayers.end() )
            mPlayers.erase( it );

        delete pPlayer;
        return true;
    }
    return false;
}

int PlayerPool::getPlayerIndex( RakNet::RakNetGUID playerid )
{
	int i = 0;
	for (int i = 0; i < GameCore::mPlayerPool->getNumberOfPlayers(); i ++ )
	{
        if( mPlayers[i]->getPlayerGUID() == playerid )
			return i;
	}

	return -1;
}

bool PlayerPool::cmpRound(Player* a, Player* b)
{
    if ( a->isAI() && !b->isAI() )
        // if player a is AI, output a higher score for b
        return true;

    if ( b->isAI() && !a->isAI() )
        // if player b is AI, output a higher score for a
        return false;

	return a->getRoundScore() < b->getRoundScore();
}

bool PlayerPool::cmpGame(Player* a, Player* b)
{
    if ( a->isAI() && !b->isAI() )
        // if player a is AI, output a score that is less than b
        return true;

    if ( b->isAI() && !a->isAI() )
        // if player b is AI, output a higher score for player a
        return false;

    return a->getGameScore() < b->getGameScore();
}

//This returns all the players in the pool in sorted score order.
std::vector<Player*> PlayerPool::getScoreOrderedPlayers()
{
	// What a waste of all our time ... Sorting the main player pool ...
	std::vector<Player*> tmp;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		tmp.push_back((*it));
	}

	std::sort(tmp.begin(), tmp.end(), PlayerPool::cmpRound);
	
	return tmp;
}

int PlayerPool::getPlayerRankIndex(Player *p, bool *sharedRank)
{
    std::vector<Player*> ordered = this->getScoreOrderedPlayers();

    *sharedRank = false;

    if (ordered.size() == 0) return 0;

    // deal with the last (most points) player outside of the for loop
    if ( ordered[ ordered.size() - 1 ] == p )
    {
        if ( ordered.size() > 1
                && !ordered[ ordered.size() - 2 ]->isAI()
                && p->getRoundScore() == ordered[ ordered.size() - 2 ]->getRoundScore() )
        {
            *sharedRank = true;
        }

        return 0;
    }
    
    int rankIndex = 0;

    // deal with the rest of the players
    for ( int i = ordered.size() - 2; i >= 0; i-- )
    {
        if ( ordered[ i + 1 ]->getRoundScore() != ordered[ i ]->getRoundScore() )
        {
            rankIndex++;
            *sharedRank = false;
        }
        else
        {
            *sharedRank = true;
        }

        if ( ordered[ i ] == p )
        {
            if (!sharedRank
                    && i >= 1
                    && !ordered[ i - 1 ]->isAI()
                    && p->getRoundScore() == ordered[ i - 1 ]->getRoundScore() )
            {
                *sharedRank = true;
            }

            break;
        }
    }

    return rankIndex;
}

//This returns all the players in the pool in sorted game score order.
std::vector<Player*> PlayerPool::getGameScoreOrderedPlayers()
{
	// What a waste of all our time ... Sorting the main player pool ...
	std::vector<Player*> tmp;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		tmp.push_back((*it));
	}

	std::sort(tmp.begin(), tmp.end(),PlayerPool::cmpGame);
	
	return tmp;
}

Player* PlayerPool::getLocalPlayer() { return mLocalPlayer; }
RakNet::RakNetGUID PlayerPool::getLocalPlayerID() { return mLocalGUID; }
Player* PlayerPool::getPlayer( int index ) { return mPlayers[index]; }
Player* PlayerPool::getPlayer( RakNet::RakNetGUID playerid )
{
    if( playerid == mLocalGUID )
        return mLocalPlayer;

	int index = getPlayerIndex( playerid );
	if( index != -1 )
		return mPlayers[index];

	return NULL;
}

void PlayerPool::frameEvent( const float timeSinceLastFrame )
{
	for( unsigned int i = 0; i < mPlayers.size(); i ++ )
	{
		processPlayer( mPlayers[i] );
        // Since we don't have access to other player's input we won't do this for now.
        //mPlayers[i]->updateGlobalGraphics( mPlayers[i]->newInput, timeSinceLastFrame );
		// TODO: add timestamps to snapshots
        
        mPlayers[i]->updateGlobalGraphics(timeSinceLastFrame);
	}

    //processPlayer( getLocalPlayer() );
    //getLocalPlayer()->updateGlobalGraphics(timeSinceLastFrame);
}

void PlayerPool::processPlayer( Player *pPlayer )
{
	if( pPlayer->mSnapshots != NULL && pPlayer->getCar() != NULL )
	{
#if BASIC_INTERP

        CarSnapshot *currentSnap = pPlayer->getCar()->getCarSnapshot();
        CarSnapshot *restoreSnap = NULL;

        btScalar dist = currentSnap->mPosition.distance( pPlayer->mSnapshots->mPosition );

        if( dist > 3.00f )
        {
            restoreSnap = (CarSnapshot*)malloc( sizeof( CarSnapshot ) );
            memcpy( restoreSnap, pPlayer->mSnapshots, sizeof( CarSnapshot ) );
        }
        else if( dist > 0.20f )
        {          
            btVector3 interpPos = pPlayer->mSnapshots->mPosition.lerp( currentSnap->mPosition, 0.9f );
            btQuaternion interpRot = pPlayer->mSnapshots->mRotation.slerp( currentSnap->mRotation, 0.9f );

            //btVector3 interpLin = pPlayer->mSnapshots->mLinearVelocity.lerp( currentSnap->mLinearVelocity, 0.9 );
            //btVector3 interpAng = pPlayer->mSnapshots->mAngularVelocity.lerp( currentSnap->mAngularVelocity, 0.9 );
                
            //restoreSnap = new CarSnapshot( interpPos, interpRot, interpAng, interpLin, currentSnap->mWheelPosition );

            restoreSnap = new CarSnapshot( 
                interpPos, 
                interpRot, 
                pPlayer->mSnapshots->mAngularVelocity,
                pPlayer->mSnapshots->mLinearVelocity, 
                pPlayer->mSnapshots->mWheelPosition );
        }

        if( restoreSnap != NULL )
        {
	        pPlayer->getCar()->restoreSnapshot( restoreSnap );

            delete( restoreSnap );
            restoreSnap = NULL;
        }

        delete( currentSnap );
        currentSnap = NULL;

#else

        pPlayer->getCar()->restoreSnapshot( pPlayer->mSnapshots );
#endif
        delete( pPlayer->mSnapshots );
        pPlayer->mSnapshots = NULL;
    }

	 // improve the judderyness of the crown, it could still be better though
	if (pPlayer->getVIP() && pPlayer->getCar() != NULL)
		GameCore::mClientGraphics->updateVIPLocation(pPlayer->getTeam(), pPlayer->getCar()->mBodyNode->getPosition());
}

void PlayerPool::roundEnd()
{
	int i = 0;
	for (int i = 0; i < GameCore::mPlayerPool->getNumberOfPlayers(); i ++ )
	{
        mPlayers[i]->delCar();
        // SERVER SHOULD HANDLE THESE
        mPlayers[i]->setAlive( true );
        // mPlayers[i]->resetHP();
        // Just want this for game end
        //mPlayers[i]->setPlayerState( PLAYER_STATE_SPAWN_SEL );
	}

    while (Car::mClonedEntities->size() > 0)
    {
        Entity *e = Car::mClonedEntities->front();
        Car::mClonedEntities->pop_front();
        GameCore::mSceneMgr->destroyEntity(e);
    }
    
    while (Car::mMeshObjects->size() > 0)
    {
        Ogre::ResourceHandle rh = Car::mMeshObjects->front();
        Car::mMeshObjects->pop_front();
        Ogre::MeshManager::getSingletonPtr()->remove(rh);
    }

    GameCore::mClientGraphics->mGameCam->setTarget( NULL );
}

void PlayerPool::setSpectating( RakNet::RakNetGUID playerid )
{
    if( playerid == mLocalGUID )
    {
        GameCore::mClientGraphics->mGameCam->setTarget( mLocalPlayer->getCar()->mBodyNode );
    }
    int idx = getPlayerIndex( playerid );
    if( idx != -1 )
    {
        mSpectating = playerid;
        setSpectating( idx );
    }
}

void PlayerPool::setSpectating( int idx )
{
    GameCore::mClientGraphics->mGameCam->setTarget( mPlayers[idx]->getCar()->mBodyNode );
}

void PlayerPool::spectateNext( bool reverse )
{
    int curIdx = getPlayerIndex( mSpectating );
    int nextIdx = -1;

    unsigned int i = reverse ? curIdx - 1 : curIdx + 1;

    // Check for an alive player further forward in player array
	for( ; i < mPlayers.size() && i >= 0; reverse ? i -- : i ++ )
    {
        if( mPlayers[i] )
        {
            if( mPlayers[i]->getCar() && mPlayers[i]->getAlive() )
            {
                nextIdx = i;
                break;
            }
        }
    }

    // Check for an alive player wrap-around
    if( nextIdx == -1 )
    {
        for( int i = reverse ? mPlayers.size()-1 : 0; reverse ? i > curIdx : i < curIdx; reverse ? i -- : i ++ )
        {
            if( mPlayers[i] )
            {
                if(  mPlayers[i]->getCar() && mPlayers[i]->getAlive() )
                {
                    nextIdx = i;
                    break;
                }
            }
        }
    }

    // Spectate new player if one was found
    if( nextIdx != -1 )
    {
        setSpectating( nextIdx );
        mSpectating = mPlayers[nextIdx]->getPlayerGUID();
    }
}

std::vector<Player*> PlayerPool::getPlayers() {
	//Put all the players from the array into a vector
	std::vector<Player*> tmp;
	for(unsigned int i=0;i<mPlayers.size();i++)
	{
		if(mPlayers[i] != NULL)
			tmp.push_back(mPlayers[i]);
	}
	
	return tmp;
}
