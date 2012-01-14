/**
 * @file	Network.cpp
 * @brief 	Major class handling the RakNet interface
 * @author  Jamie Barber
 */

//********************//
//   CLIENT NETWORK   //
//********************//

#include "stdafx.h"
#include "NetworkClient.h"

extern PlayerPool *pPlayerPool;

CNetwork::CNetwork( char *szHost, int iPort, int iMaxPlayers, char *szPass )
{
	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult iStart = m_pRak->Startup( 1, &RakNet::SocketDescriptor(), 1 );

	RegisterRPCSlots();

	// Connect to the specified server
	bool bCon = m_pRak->Connect( szHost, iPort, szPass, szPass == NULL ? 0 : strlen(szPass) );
	m_pRak->SetOccasionalPing( true );

	if( bCon )
		log( "Connecting to %s : %i", szHost, iPort );
	else
		log( "Failed to connect to %s : %i", szHost, iPort );
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


		default:
			log( "Something else happened" );
		
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
	PLAYER_SYNC_DATA playerState;
	RakNet::BitStream bitStream( pkt->data, pkt->length, false );

	unsigned char bPacketID;
	bitStream.Read( bPacketID );

	bitStream.Read( (char*)&playerState );

	PlayerState psUpdate( playerState.vLocation, playerState.fSpeed, playerState.fRotation, playerState.fAcceleration );

	// THIS IS WHERE WE SHALL ADD IN SOME UPDATING CODE
}

// ---------- RPC CALLS ----------
//
// Called remotely by clients
// BitStream contains the data sent with the call
// Packet can be used for determining player and replying
//
// -------------------------------

/*
	GameJoin RPC Call
		Receiving game init parameters from the server
*/
void CNetwork::GameJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	int iPlayerID;
	char szNickname[128];

	bitStream->Read( iPlayerID );
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );
	
	// Add the player to the pool, and set our local player ID
	pPlayerPool->iLocalPlayer = iPlayerID;
	pPlayerPool->addPlayer( iPlayerID, szNickname );
}

/*
	Player Join RPC Call
		Called by server when a player joins
		Create entry in local player pool
*/
void CNetwork::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	int iPlayerID;
	char szNickname[128];

	bitStream->Read( iPlayerID );
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );

	pPlayerPool->addPlayer( iPlayerID, szNickname );
}

/*
	Player Quit RPC Call
		For gracious game exit
		Clean up after the player (remove from pool)
*/
void CNetwork::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	int iPlayerID;
	bitStream->Read( iPlayerID );

	pPlayerPool->delPlayer( iPlayerID );
}

/*
	Player Chat RPC Call
		Called when we (as client) receive a chat message
		Also look at team and private messaging
*/
void CNetwork::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	int iPlayerID;
	char szMessage[512];
	bitStream->Read( iPlayerID );
	RakNet::StringCompressor().DecodeString( szMessage, 512, bitStream );
}

/*
	Player Spawn RPC Call
		Called when a player spawns
		Alert graphics to create a new mesh etc
*/
void CNetwork::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	int iPlayerID;
	bitStream->Read( iPlayerID );
}

void CNetwork::RegisterRPCSlots()
{
	m_pRak->AttachPlugin( &m_RPC );

	m_RPC.RegisterSlot( "GameJoin",			GameJoin, 0 );
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