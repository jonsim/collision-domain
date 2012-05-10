#ifndef PLAYERPOOL_H
#define PLAYERPOOL_H

#define MAX_PLAYERS 100

#include "stdafx.h"
#include "Player.h"

// RakNet includes
#include "RakNetTypes.h"
#include <vector>
#include <limits>

class Player;

class PlayerPool
{
private:
	std::vector<Player*> mPlayers;
	Player* mLocalPlayer;
	RakNet::RakNetGUID mLocalGUID;
	int getPlayerIndex( RakNet::RakNetGUID playerid );

public:
	PlayerPool();
	~PlayerPool();
	
	int addPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	void addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname );
	bool delPlayer( RakNet::RakNetGUID playerid );
	int getNumberOfPlayers();
	Player* getRandomPlayer();
	Player* getClosestPlayer(Player* player);
	std::vector<Player*> getPlayers() { return mPlayers;};
	static bool cmpRound(Player* a, Player* b); //Sorts the players based on their round score
    static bool cmpGame(Player* a, Player* b); //Sorts the players based on their game score
	std::vector<Player*> getScoreOrderedPlayers();
    std::vector<Player*> getGameScoreOrderedPlayers();

	Player* getPlayer( int index );
	Player* getPlayer( RakNet::RakNetGUID playerid );
	Player* getPlayer( const char* nickname);
	RakNet::RakNetGUID getPlayerGUID( int index );

	//get the enemy vip player
	Player *getEnemyVip(int team);

	void frameEvent( const float timeSinceLastFrame );
    void roundEnd();
};


#endif
