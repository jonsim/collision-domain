#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#define MAX_PLAYERS 100

#include "stdafx.h"
#include "GameIncludes.h"

// RakNet includes
#include "RakNetTypes.h"

class Player;

class PlayerPool
{
private:
	Player* mPlayers[MAX_PLAYERS];
	Player* mLocalPlayer;
	RakNet::RakNetGUID mLocalGUID;
	RakNet::RakNetGUID mGUID[MAX_PLAYERS];
	int getPlayerIndex( RakNet::RakNetGUID playerid );

public:
	PlayerPool();
	~PlayerPool();
	
	int addPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void delPlayer( RakNet::RakNetGUID playerid );
	int getNumberOfPlayers();

	Player* getPlayer( int index );
	Player* getPlayer( RakNet::RakNetGUID playerid );
	//Player* getLocalPlayer();
	//RakNet::RakNetGUID getLocalPlayerID();
	RakNet::RakNetGUID getPlayerGUID( int index );


	void frameEvent( const Ogre::FrameEvent& evt );
	//LocalPlayer *getLocalPlayer() { return static_cast<LocalPlayer>(m_pPlayers[iLocalPlayer]); }
};

#endif