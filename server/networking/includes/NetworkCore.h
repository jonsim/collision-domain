/**
 * @file	NetworkCore.h
 * @brief 	Takes notifications and deals with them however networking needs to
 */
#ifndef NETWORKCORE_H
#define NETWORKCORE_H

/*-------------------- INCLUDES --------------------*/

// Things just defined here, need to change later
#define SERVER_PORT 55010
#define SERVER_PASS 0
#define ENCRYPT_DATA 0
#define UPDATE_INTERVAL 20
#define LOG_FILENAME "cdomain.txt"

// Game includes
#include "stdafx.h"
#include "Powerup.h"
#include "CarSnapshot.h"
#include "SceneSetup.h"

// RakNet includes
#include "BitStream.h"

#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "StringCompressor.h"
#include "WindowsIncludes.h"
#include "GetTime.h"

// RakNet plugins
#include "RPC4Plugin.h"

// THIS SHOULDN'T BE HERE, FOR LATER, SOMEWHERE MORE GLOBAL (I find it useful)
void log( char *data, ... );

/*-------------------- CLASS DEFINITIONS --------------------*/
/**
 * @brief       A class to take notifications and deal with them however networking needs to
 *
 */

// Define our custom packet ID's
enum
{
	ID_PLAYER_SNAPSHOT = ID_USER_PACKET_ENUM,
	ID_PLAYER_INPUT,
    ID_PLAYER_DAMAGE,
    ID_SPAWN_SUCCESS,
    ID_SPAWN_NO_TEAM,
    ID_SPAWN_GAME_INACTIVE,
    ID_SPAWN_WAIT_NEXT_GAME,
};

struct SERVER_INFO_DATA
{
    unsigned int publicSeed;
    int curMap;
};

struct PLAYER_INPUT_DATA
{
	bool frwdPressed;
	bool backPressed;
	bool leftPressed;
	bool rghtPressed;
    bool hndbPressed;
};

struct PLAYER_SYNC_DATA
{
	RakNet::RakNetGUID playerid;
	RakNet::Time timestamp;
	btVector3 vPosition;
    btQuaternion qRotation;
    btVector3 vAngVel;
    btVector3 vLinVel;
	float fWheelPos;
};

class InfoItem;

class NetworkCore
{
private:
	static RakNet::RakPeerInterface *m_pRak;
	static RakNet::RPC4 *m_RPC;
	static RakNet::TimeMS timeLastUpdate;

    static SERVER_INFO_DATA serverInfo;

public:
    NetworkCore();
    ~NetworkCore (void);
	void init( char *szPass );

	RakNet::RakPeerInterface* getRakInterface();
	void RegisterRPCSlots();

    void frameEvent();
	void ProcessPlayerState( RakNet::Packet *pkt );
	void BroadcastUpdates();
	void BroadcastRPC( char *rpcName, RakNet::BitStream *bsData );
	static void GamestateUpdatePlayer( RakNet::RakNetGUID playerid );
	static void SetupGameForPlayer( RakNet::RakNetGUID playerid );
    static void HandlePlayerQuit( RakNet::RakNetGUID playerid, unsigned char reason );

	void sendInfoItem(InfoItem* ii, bool show);
	void sendPlayerDeath(Player* player, Player* causedBy); 
	void declareNewVIP(Player* player);

    void sendPlayerSpawn( Player *pPlayer );
    void sendPowerupCreate( int pwrID, PowerupType pwrType, Ogre::Vector3 pwrLoc );
    void sendPowerupCollect( int pwrID, Player *player, float extraData );
    void sendChatMessage( const char *szMessage );
    void sendSyncScores();
    void sendGameSync(GameMode gameMode, ArenaID arenaID);
    void sendTimeSinceRoundStart(time_t startTime);

    CarSnapshot* getCarSnapshotIfExistsSincePreviousGet(int playerID);

	// RPC Calls
	static void GameJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void InfoItemTransmit( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
    static void PlayerTeamSelect( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	

	static bool bConnected;

};

#endif // #ifndef NETWORKCORE_H
