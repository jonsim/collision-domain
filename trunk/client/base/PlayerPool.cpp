#include "stdafx.h"
#include "PlayerPool.h"

PlayerPool::PlayerPool()
{
	// Initialize the pool
	for( int i = 0; i < MAX_PLAYERS; i ++ )
	{
		m_pPlayers[i] = NULL;
	}
}
void PlayerPool::addPlayer( int iPlayerID, char *szNickname )
{
	// Something has gone wrong
	if( m_pPlayers[iPlayerID] != NULL )
		return;

	m_pPlayers[iPlayerID] = new Player();
}

void PlayerPool::addLocalPlayer( int iPlayerID, char *szNickname )
{
	if( m_pPlayers[iPlayerID] != NULL )
		return;
}

void PlayerPool::delPlayer( int iPlayerID )
{
	delete m_pPlayers[iPlayerID];
	m_pPlayers[iPlayerID] = NULL;
}