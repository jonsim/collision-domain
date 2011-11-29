/**
 * @file	Network.cpp
 * @brief 	Major class handling the RakNet interface
 * @author  Jamie Barber
 */

//********************//
//   SERVER NETWORK   //
//********************//


#include "Network.h"

CNetwork::CNetwork( int iPort, int iMaxPlayers, char *szPass = NULL )
{
	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult iStart = m_pRak->Startup( MAX_PLAYERS, &RakNet::SocketDescriptor(SERVER_PORT, 0), 1 );

	// Report any error starting the server
	if( iStart != RakNet::RAKNET_STARTED )
	{
		printf( "Could not start server. Please check the port is not already in use (port %d)\n", SERVER_PORT );
		return;
	}

	// Allow incoming connections, turn on occasional ping
	m_pRak->SetMaximumIncomingConnections( MAX_PLAYERS );
	m_pRak->SetOccasionalPing( true );

	// Set the server password if one is specified
	if( szPass != NULL )
		m_pRak->SetIncomingPassword( szPass, strlen( szPass ) );

	RegisterRPCSlots();
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
		case ID_NEW_INCOMING_CONNECTION:
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			break;
		case ID_CONNECTION_LOST:
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
	//Some pseudo code for how this will work

#ifdef blabla

	// Create a struct to contain player state
	CPlayerState playerState = new CPlayerState();
	pPlayerPool *CPlayerPool;

	// Get the player in question out of the player pool
	CPlayer *pPlayer = CPlayerPool->getPlayer(pkt->guid);

	// Cast received data into player state struct
	playerState = (CPlayerState)&pkt->data[1];

	// Set the new player state info
	pPlayer->setPlayerState( &playerState );

	// Some flag that this player's state has changed
	pPlayer->setSendUpdate( true );

	// New state will then be broadcast on the next server tick
	// after processing physics
	return;

#endif

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
		Set up the required data for that player
		Send them initial game state
*/
void CNetwork::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
}

/*
	Player Quit RPC Call
		For gracious game exit
		Clean up after the player (remove from pool)
*/
void CNetwork::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
}

/*
	Player Chat RPC Call
		Handle sending chat messages between players
		Also look at team and private messaging
*/
void CNetwork::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
}

/*
	Player Spawn RPC Call
		Send player map state and a full update
		Any local processing that needs doing
*/
void CNetwork::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
}

/*
	Player Requesting Full Update RPC
		Possibly not needed but..
		If client locally knows it's out of sync, we can allow it to
		request an update of entire game and map state from the server
*/
void CNetwork::RequestFullUpdate( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
}

void CNetwork::RegisterRPCSlots()
{
	m_pRak->AttachPlugin( &m_RPC );

	m_RPC.RegisterSlot( "PlayerJoin",		PlayerJoin, 0 );
	m_RPC.RegisterSlot( "PlayerQuit",		PlayerQuit, 0 );
	m_RPC.RegisterSlot( "PlayerChat",		PlayerChat, 0 );
	m_RPC.RegisterSlot( "PlayerSpawn",		PlayerSpawn, 0 );
	m_RPC.RegisterSlot( "RequestFullUpdate",RequestFullUpdate, 0 );
}
