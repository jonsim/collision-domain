#include "stdafx.h"
#include "GameIncludes.h"

PlayerPool::PlayerPool() : mLocalPlayer(0)
{

}

int PlayerPool::addPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	int iNew = mPlayers.size();
	mPlayers.push_back(new Player());
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
	mLocalPlayer = new Player();
	mLocalPlayer->setPlayerGUID(playerid);
	mLocalPlayer->setNickname(szNickname);
	mLocalGUID = playerid;
	
	mPlayers.push_back(mLocalPlayer);
	//mGUID.push_back(playerid);

}

bool PlayerPool::delPlayer( RakNet::RakNetGUID playerid )
{
	Player *pPlayer = getPlayer( playerid );
	if( pPlayer )
	{
        std::vector<Player*>::iterator it = find( mPlayers.begin(), mPlayers.end(), pPlayer );
        if( it != mPlayers.end() )
        {
            //if (*it != pPlayer) assert(false);
            mPlayers.erase( it );
        }

<<<<<<< .mine
        //std::vector<RakNet::RakNetGUID>::iterator it2 = find( mGUID.begin(), mGUID.end(), playerid );
        //if( it2 != mGUID.end() )
        //    mGUID.erase( it2 );
=======
        std::vector<RakNet::RakNetGUID>::iterator it2 = find( mGUID.begin(), mGUID.end(), playerid );
        if( it2 != mGUID.end() )
        {
            //if (*it2 != playerid) assert(false);
            mGUID.erase( it2 );
>>>>>>> .r493
        }

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
		//if( mGUID[i] == playerid && mPlayers[i] != NULL )
        if( mPlayers[i]->getPlayerGUID() == playerid )
			return i;
	}

	return -1;
}

Player* PlayerPool::getRandomPlayer()
{
	int nPlayers = getNumberOfPlayers();
	int i = rand() % (nPlayers + 1);

	return mPlayers[i];
}

Player* PlayerPool::getClosestPlayer(Player* player)
{
	double minDist = numeric_limits<double>::infinity(), dist;
	Player* closest = NULL;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		//closet player will be ourselves, therefore ignore
		if(player->getTeam() == (*it)->getTeam())
			continue;

		if((*it)->isReady())
			dist = (player->getCar()->GetPos()).distance((*it)->getCar()->GetPos());
		else
			continue;

		if(dist < minDist)
		{
			minDist = dist;
			closest = (Player*)(*it);
		}
	}

	return closest;
}

bool PlayerPool::cmpRound(Player* a, Player* b)
{
	return a->getRoundScore() < b->getRoundScore();
}

bool PlayerPool::cmpGame(Player* a, Player* b)
{
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

	std::sort(tmp.begin(),tmp.end(),PlayerPool::cmpRound);
	
	return tmp;
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

	std::sort(tmp.begin(),tmp.end(),PlayerPool::cmpGame);
	
	return tmp;
}


RakNet::RakNetGUID PlayerPool::getPlayerGUID( int index ) { return mPlayers[index]->getPlayerGUID(); /*mGUID[index];*/ }

Player* PlayerPool::getPlayer( int index ) { return mPlayers[index]; }

Player* PlayerPool::getPlayer( RakNet::RakNetGUID playerid )
{
	int index = getPlayerIndex( playerid );
	if( index != -1 )
		return mPlayers[index];

	return NULL;
}

Player* PlayerPool::getPlayer( const char* nickname)
{
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		if(!(strcmp(((Player*)(*it))->getNickname(), nickname)))
			return (Player*)(*it);
	}

	return NULL;
}

void PlayerPool::frameEvent( const float timeSinceLastFrame )
{
	for( int i = 0; i < GameCore::mPlayerPool->getNumberOfPlayers(); i ++ )
	{
		// Local player physics in GraphicsApplication
		//if( mGUID[i] == mLocalGUID )
		//	continue;
		if( mPlayers[i] == NULL )
			return;
		if( mPlayers[i]->newInput != NULL )
			mPlayers[i]->processControlsFrameEvent( mPlayers[i]->newInput, timeSinceLastFrame, (1.0f / 60.0f));
	}

}


