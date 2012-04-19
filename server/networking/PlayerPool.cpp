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
	mGUID.push_back(playerid);
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
	mGUID.push_back(playerid);

}

void PlayerPool::delPlayer( RakNet::RakNetGUID playerid )
{
	int iRemove = getPlayerIndex( playerid );
	if( iRemove != -1 )
	{
		delete mPlayers[iRemove];
		mPlayers[iRemove] = NULL;
	}
}

int PlayerPool::getPlayerIndex( RakNet::RakNetGUID playerid )
{
	int i = 0;
	for (int i = 0; i < GameCore::mPlayerPool->getNumberOfPlayers(); i ++ )
	{
		if( mGUID[i] == playerid )
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
	//This is a much nicer way of doing it
	std::sort(this->mPlayers.begin(),this->mPlayers.end(),PlayerPool::cmpRound);
	
	std::vector<Player*> tmp;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		tmp.push_back((*it));
	}
	
	//return this->mPlayers;
	return tmp;
}


//This returns all the players in the pool in sorted game score order.
std::vector<Player*> PlayerPool::getGameScoreOrderedPlayers()
{
	//This is a much nicer way of doing it
	std::sort(this->mPlayers.begin(),this->mPlayers.end(),PlayerPool::cmpGame);
	
    //In theory this is not required
	std::vector<Player*> tmp;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		tmp.push_back((*it));
	}
	
	//return this->mPlayers;
	return tmp;
}


RakNet::RakNetGUID PlayerPool::getPlayerGUID( int index ) { return mGUID[index]; }

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
		if( mGUID[i] == mLocalGUID )
			continue;
		if( mPlayers[i] == NULL )
			return;
		if( mPlayers[i]->newInput != NULL )
			mPlayers[i]->processControlsFrameEvent( mPlayers[i]->newInput, timeSinceLastFrame, (1.0f / 60.0f));
	}

}


