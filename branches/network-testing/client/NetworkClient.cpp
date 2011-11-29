/**
 * @file	Network.cpp
 * @brief 	Major class handling the RakNet interface
 * @author  Jamie Barber
 */

//********************//
//   CLIENT NETWORK   //
//********************//


#include "NetworkClient.h"

CNetwork::CNetwork( char *szHost, int iPort, int iMaxPlayers, char *szPass = NULL )
{
	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult iStart = m_pRak->Startup( 1, &RakNet::SocketDescriptor(), 1 );

	// Report any error starting the server
	if( iStart != RakNet::RAKNET_STARTED )
	{
		printf( "Could not start server. Please check the port is not already in use (port %d)\n", SERVER_PORT );
		return;
	}

	RegisterRPCSlots();

	// Connect to the specified server
	m_pRak->Connect( szHost, iPort, szPass, strlen(szPass) );
	m_pRak->SetOccasionalPing( true );
}

CNetwork::~CNetwork()
{
	RakNet::RakPeerInterface::DestroyInstance( m_pRak );
}

RakNet::RakPeerInterface* CNetwork::getRakInterface()
{
	// Return our interface to RakNet
	// Shouldn't ever really need this as long as this class
	// implements most things that would ne required
	return m_pRak;
}

void CNetwork::Process()
{
	RakNet::Packet *pkt;

	for( pkt = m_pRak->Receive(); pkt; m_pRak->DeallocatePacket(pkt), pkt=m_pRak->Receive() );
	{
		switch( pkt->data[0] )
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			log( "Connection to server accepted" );
			break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			log( "Connection to server FAILED" );
			break;
		case ID_ALREADY_CONNECTED:
			log( "Already connected to specified server" );
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			log( "Server appears to be full" );
			break;

		case ID_INVALID_PASSWORD:
			log( "Invalid password specified" );
			break;
		case ID_CONNECTION_BANNED:
			log( "Banned from this server" );
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			log( "Disconnected from server" );
			break;
		case ID_CONNECTION_LOST:
			log( "Lost connection to server" );
			break;

		case ID_PLAYER_STATE:
			ProcessPlayerState( pkt );
			break;
		
		// Don't *think* these are needed
		/*case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			break;
		case ID_REMOTE_CONNECTION_LOST:
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			break;*/
		}
	}
}

/*
	Called when a player state has been received
	Update the player's info in the pool
	(by player state I mean key state really)
*/
void CNetwork::ProcessPlayerState( RakNet::Packet *pkt )
{
	// Here marks the beginning of crazy interpolation to 
	// smoothly "rewind and replay" previous physics frames
}

// ---------- RPC CALLS ----------
//
// Called remotely by clients
// BitStream contains the data sent with the call
// Packet can be used for determining player and replying
//
// -------------------------------

/*
	Player Join RPC Call
		Called by server when a player joins
		Create entry in local player pool
*/
void CNetwork::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
#ifdef blablabla

	CPlayerPool *m_pPlayerPool;
	m_pPlayerPool->add( pkt->guid );

	struct playerinfo{ id, name };
	pPlayerPool->setPlayerInfo( ParsePlayerInfo( bitStream ) );

#endif
}

/*
	Player Quit RPC Call
		For gracious game exit
		Clean up after the player (remove from pool)
*/
void CNetwork::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
#ifdef blablabla

	m_pPlayerPool->getPlayer( pkt->guid )->cleanup(); // Remove assets from gfx/phys engine
	m_pPlayerPool->remove( pkt->guid );

#endif
}

/*
	Player Chat RPC Call
		Called when we (as client) receive a chat message
		Also look at team and private messaging
*/
void CNetwork::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
#ifdef blablabla

	char *szPlayerName;
	char szMsg[256];

	bitStream->ReadCompressed( szMsg );
	szPlayerName = m_pPlayerPool->getPlayer( pkt->guid )->getPlayerInfo().nickname;

	m_gameDisplay->addChatMessage( szPlayerName, szMsg );

#endif
}

/*
	Player Spawn RPC Call
		Called when a player spawns
		Alert graphics to create a new mesh etc
*/
void CNetwork::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
#ifdef blablabla

	CPlayer *pPlayer = m_pPlayerPool->get( pkt->guid );

	struct spawndata { x, y, z, carmodel, skin, team };
	pPlayer->Spawn( ParseSpawnData( bitStream ) );

#endif
}

void CNetwork::RegisterRPCSlots()
{
	m_pRak->AttachPlugin( &m_RPC );

	m_RPC.RegisterSlot( "PlayerJoin",		PlayerJoin, 0 );
	m_RPC.RegisterSlot( "PlayerQuit",		PlayerQuit, 0 );
	m_RPC.RegisterSlot( "PlayerChat",		PlayerChat, 0 );
	m_RPC.RegisterSlot( "PlayerSpawn",		PlayerSpawn, 0 );
}

// THIS SHOULDN'T BE HERE, FOR LATER
void log( char *data, ... )
{
	char buffer[2048];
	int error = 0;

    va_list va;
	va_start(va, data);
	vsprintf(buffer, data, va);
	va_end(va);
	strcat(buffer, "\n");

	FILE *fLog = fopen( LOG_FILENAME, "a" );
	fwrite( buffer, 1, strlen( buffer ), fLog );
	fclose( fLog );
	
	return;
}