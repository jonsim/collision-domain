#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#define MAX_PLAYERS 100

#include "stdafx.h"
#include "RakNetTypes.h" // RakNet includes

class Player;

class PlayerPool
{
private:
	std::vector<Player*> mPlayers;
	//std::vector<RakNet::RakNetGUID> mGUID;
	Player*             mLocalPlayer;
	RakNet::RakNetGUID  mLocalGUID;
	int                 getPlayerIndex( RakNet::RakNetGUID playerid );
    RakNet::RakNetGUID  mSpectating;
    void                setSpectating( int idx );

public:
	PlayerPool();
	~PlayerPool();
	
	int                addPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void                addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	bool                delPlayer( RakNet::RakNetGUID playerid );
	int                 getNumberOfPlayers();
	static bool cmpRound(Player* a, Player* b); //Sorts the players based on their round score
    static bool cmpGame(Player* a, Player* b); //Sorts the players based on their game score
	std::vector<Player*> getScoreOrderedPlayers();
    std::vector<Player*> getGameScoreOrderedPlayers();
	std::vector<Player*> getPlayers();
    int                  getPlayerRankIndex(Player *p);

	Player*             getPlayer( int index );
	Player*             getPlayer( RakNet::RakNetGUID playerid );
	Player*             getLocalPlayer();
	RakNet::RakNetGUID  getLocalPlayerID();

    RakNet::RakNetGUID  getSpectating() { return mSpectating; }
    void                setSpectating( RakNet::RakNetGUID playerid );
    void                spectateNext( bool reverse = false );

	void                frameEvent( const float timeSinceLastFrame );
    void                roundEnd();

    void                processPlayer( Player *pPlayer );
	//LocalPlayer *getLocalPlayer() { return static_cast<LocalPlayer>(m_pPlayers[iLocalPlayer]); }
};

#endif
