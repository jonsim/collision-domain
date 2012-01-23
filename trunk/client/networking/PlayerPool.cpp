
#include "stdafx.h"
#include "GameIncludes.h"

PlayerPool::PlayerPool()
{
	// Initialize the pool
	for( int i = 0; i < MAX_PLAYERS; i ++ )
	{
		mPlayers[i] = NULL;
	}
}

void PlayerPool::addPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	int i = 0, iNew = -1;
	for( i = 0; i < MAX_PLAYERS; i ++ )
	{
		if( mPlayers[i] == NULL )
		{
			iNew = i;
			break;
		}
	}

	if( iNew != -1 )
	{
		mPlayers[iNew] = new Player();
		mGUID[iNew] = playerid;
	}
}

void PlayerPool::addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	mLocalPlayer = new Player();
	mLocalGUID = playerid;
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

Player* PlayerPool::getLocalPlayer() { return mLocalPlayer; }
RakNet::RakNetGUID PlayerPool::getLocalPlayerID() { return mLocalGUID; }

Player* PlayerPool::getPlayer( RakNet::RakNetGUID playerid )
{
	int index = getPlayerIndex( playerid );
	if( index != -1 )
		return mPlayers[index];

	return NULL;
}

void PlayerPool::frameEvent()
{
	int i = 0;
	Player *pPlayer;

	for( i = 0; i < MAX_PLAYERS; i ++ )
	{
		// Skip over the local player for now
		if( mGUID[i] == mLocalGUID )
			continue;

		pPlayer = mPlayers[i];
		if( pPlayer == NULL )
			continue;

		if( pPlayer->mSnapshots != NULL && pPlayer->getCar() != NULL )
		{
			pPlayer->getCar()->restoreSnapshot( pPlayer->mSnapshots );
			delete( pPlayer->mSnapshots );
			pPlayer->mSnapshots = NULL;
		}
		// TODO: add timestamps to snapshots
	}

	// Perform local client update
	if( mLocalPlayer != NULL )
	{
		if( mLocalPlayer->getCar() != NULL && mLocalPlayer->mSnapshots != NULL )
		{
			mLocalPlayer->getCar()->restoreSnapshot( mLocalPlayer->mSnapshots );
			delete( mLocalPlayer->mSnapshots );
			mLocalPlayer->mSnapshots = NULL;
		}
	}

}