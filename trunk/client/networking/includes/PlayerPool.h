#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#define MAX_PLAYERS 100

#include "stdafx.h"
#include "GameIncludes.h"
#include "RakNetTypes.h" // RakNet includes

class Player;

class PlayerPool
{
private:
	Player*             mPlayers[MAX_PLAYERS];
	Player*             mLocalPlayer;
	RakNet::RakNetGUID  mLocalGUID;
	RakNet::RakNetGUID  mGUID[MAX_PLAYERS];
	int                 getPlayerIndex( RakNet::RakNetGUID playerid );
    RakNet::RakNetGUID  mSpectating;
    void                setSpectating( int idx );

public:
	PlayerPool();
	~PlayerPool();
	
	void                addPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void                addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void                delPlayer( RakNet::RakNetGUID playerid );
	int                 getNumberOfPlayers();
	static bool cmp(Player* a, Player* b);
	std::vector<Player*> getScoreOrderedPlayers();
	std::vector<Player*> getPlayers();

	Player*             getPlayer( int index );
	Player*             getPlayer( RakNet::RakNetGUID playerid );
	Player*             getLocalPlayer();
	RakNet::RakNetGUID  getLocalPlayerID();

    RakNet::RakNetGUID  getSpectating() { return mSpectating; }
    void                setSpectating( RakNet::RakNetGUID playerid );
    void                spectateNext();

	void                frameEvent( const float timeSinceLastFrame );

    void                processPlayer( Player *pPlayer );
	//LocalPlayer *getLocalPlayer() { return static_cast<LocalPlayer>(m_pPlayers[iLocalPlayer]); }
};

#endif