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
	for (int i = 0; i < MAX_PLAYERS; i ++ )
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
	Player* closest;
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		//closet player will be ourselves, therefore ignore
		if(player->getTeam() == (*it)->getTeam())
			continue;

		dist = (player->getCar()->GetPos()).distance((*it)->getCar()->GetPos());

		if(dist < minDist)
		{
			minDist = dist;
			closest = (Player*)(*it);
		}
	}

	return closest;
}

bool PlayerPool::cmp(Player* a, Player* b)
{
	return a->getRoundScore() < b->getRoundScore();
}

//This returns all the players in the pool in sorted score order.
std::vector<Player*> PlayerPool::getScoreOrderedPlayers()
{
	//This is a much nicer way of doing it
	std::sort(this->mPlayers.begin(),this->mPlayers.end(),PlayerPool::cmp);
	
	std::vector<Player*> tmp;
	OutputDebugString("Sorted List");
	for(std::vector<Player*>::iterator it = mPlayers.begin();it != mPlayers.end();it++)
	{
		std::stringstream tmpString;
		tmpString << "Player: " << (*it)->getNickname() << " " << (*it)->getRoundScore() << "\n";
		OutputDebugString(tmpString.str().c_str());
		tmp.push_back((*it));
	}
	
	//return this->mPlayers;
	return tmp;
}

Player* PlayerPool::getLocalPlayer() { return mLocalPlayer; }
RakNet::RakNetGUID PlayerPool::getLocalPlayerID() { return mLocalGUID; }
RakNet::RakNetGUID PlayerPool::getPlayerGUID( int index ) { return mGUID[index]; }

Player* PlayerPool::getPlayer( int index ) { return mPlayers[index]; }

Player* PlayerPool::getPlayer( RakNet::RakNetGUID playerid )
{
	int index = getPlayerIndex( playerid );
	if( index != -1 )
		return mPlayers[index];

	return NULL;
}

void PlayerPool::frameEvent( const Ogre::FrameEvent& evt )
{
	int i = 0;
	Player *pPlayer;
	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	for( i = 0; i < size; i ++ )
	{
		// Local player physics in GraphicsApplication
		if( mGUID[i] == mLocalGUID )
			continue;

		pPlayer = mPlayers[i];
		if( pPlayer == NULL )
			return;
		if( pPlayer->newInput != NULL )
		{
			// This might cause problems.
			// and it did, beautifully.
			// Unhandled exception at 0x7798e653 in collision_server.exe: 0xC0000374: A heap has been corrupted.
			pPlayer->processControlsFrameEvent( pPlayer->newInput, evt.timeSinceLastFrame, (1.0f / 60.0f));
		}
		// TODO: add timestamps to snapshots
        
        if (pPlayer->getVIP())
            GameCore::mGraphicsCore->updateVIPLocation(pPlayer->getTeam(), pPlayer->getCar()->mBodyNode->getPosition());
	}

}