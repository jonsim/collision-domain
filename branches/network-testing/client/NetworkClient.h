/**
 * @file	Network.cpp
 * @brief 	Major class handling the RakNet interface
 * @author  Jamie Barber
 */

//********************//
//   CLIENT NETWORK   //
//********************//

// RakNet includes
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "RakPeerInterface.h"

// RakNet plugins
#include "RPC4Plugin.h"

// Things just defined here, need to change later
#define MAX_PLAYERS 50
#define SERVER_PORT 55010
#define SERVER_PASS 0
#define ENCRYPT_DATA 0


// THIS SHOULDN'T BE HERE, FOR LATER
#define LOG_FILENAME "cdomain.txt"
void log( char *data, ... );

// Define our custom packet ID's
enum
{
	ID_PLAYER_STATE = ID_USER_PACKET_ENUM,

};


class CNetwork
{
private:

	RakNet::RakPeerInterface *m_pRak;
	RakNet::RPC4 m_RPC;

public:

	CNetwork( char *szHost, int iPort, int iMaxPlayers, char *szPass = NULL );
	~CNetwork();

	RakNet::RakPeerInterface* getRakInterface();

	void RegisterRPCSlots();

	void Process();
	void ProcessPlayerState( RakNet::Packet *pkt );

	// RPC Calls
	static void PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt );
	static void PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt );

};
