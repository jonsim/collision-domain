#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#define MAX_PLAYERS 100

#include "stdafx.h"
#include "ClientIncludes.h"
#include "RakNetTypes.h" // RakNet includes

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
	
	void addPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void delPlayer( RakNet::RakNetGUID playerid );

	Player* getPlayer( RakNet::RakNetGUID playerid );
	Player* getLocalPlayer();
	RakNet::RakNetGUID getLocalPlayerID();


	void frameEvent();
	//LocalPlayer *getLocalPlayer() { return static_cast<LocalPlayer>(m_pPlayers[iLocalPlayer]); }
};

#endif